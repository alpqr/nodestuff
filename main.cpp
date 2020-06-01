#include <QGuiApplication>
#include <QQuickView>
#include "qrhiimgui.h"
#include "gui.h"

struct ImGuiQuick
{
    ImGuiQuick();
    ~ImGuiQuick();

    void setWindow(QQuickWindow *window);
    void setVisible(bool b);

    void init();
    void release();
    void prepare();
    void render();

    QRhiImgui d;
    QQuickWindow *w = nullptr;
    QRhi *rhi = nullptr;
    QRhiSwapChain *swapchain = nullptr;
    bool visible = true;
};

ImGuiQuick::ImGuiQuick()
{
    d.setDepthTest(false);
}

ImGuiQuick::~ImGuiQuick()
{
    release();
}

void ImGuiQuick::setWindow(QQuickWindow *window)
{
    d.setInputEventSource(window);
    d.setEatInputEvents(true);
    w = window;
}

void ImGuiQuick::setVisible(bool b)
{
    if (visible == b)
        return;

    visible = b;
    d.setEatInputEvents(visible);
}

void ImGuiQuick::init()
{ // render thread
    QSGRendererInterface *rif = w->rendererInterface();
    rhi = static_cast<QRhi *>(rif->getResource(w, QSGRendererInterface::RhiResource));
    if (!rhi)
        qFatal("Failed to retrieve QRhi from QQuickWindow");
    swapchain = static_cast<QRhiSwapChain *>(rif->getResource(w, QSGRendererInterface::RhiSwapchainResource));
    if (!swapchain)
        qFatal("Failed to retrieve QRhiSwapChain from QQuickWindow");

    d.initialize(rhi);
}

void ImGuiQuick::release()
{ // render thread
    d.releaseResources();
    w = nullptr;
}

void ImGuiQuick::prepare()
{ // render thread
    if (!w)
        return;

    QRhiResourceUpdateBatch *u = rhi->nextResourceUpdateBatch();
    d.prepareFrame(swapchain->currentFrameRenderTarget(), swapchain->renderPassDescriptor(), u);
    swapchain->currentFrameCommandBuffer()->resourceUpdate(u);
}

void ImGuiQuick::render()
{ // render thread
    if (!w || !visible)
        return;

    d.queueFrame(swapchain->currentFrameCommandBuffer());
}

int main(int argc, char *argv[])
{
    qputenv("QSG_INFO", "1");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    Gui gui;
    ImGuiQuick ig;
    QQuickView view;

    QObject::connect(&view, &QQuickWindow::sceneGraphInitialized, &view, [&ig] { ig.init(); }, Qt::DirectConnection);
    QObject::connect(&view, &QQuickWindow::sceneGraphInvalidated, &view, [&ig] { ig.release(); }, Qt::DirectConnection);
    QObject::connect(&view, &QQuickWindow::beforeRendering, &view, [&ig] { ig.prepare(); }, Qt::DirectConnection);
    QObject::connect(&view, &QQuickWindow::afterRenderPassRecording, &view, [&ig] { ig.render(); }, Qt::DirectConnection);

    ImGui::GetIO().IniFilename = nullptr;
    ig.setWindow(&view);
    ig.d.setFrameFunc([&gui] { gui.frame(); });

    view.setColor(Qt::black);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.resize(1280, 720);
    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    int r = app.exec();

    return r;
}
