#ifndef GRAPHEVAL_H
#define GRAPHEVAL_H

#include <vector>
#include "portdata.h"

struct Graph;
struct Node;

namespace GraphEval {

void update(Graph &g);

using EvalStackType = std::vector<PortData>;

void evalConstantNode(Graph &g, Node &n, EvalStackType &evalStack);

void evalPlusNode(Graph &g, Node &n, EvalStackType &evalStack);
void evalMinusNode(Graph &g, Node &n, EvalStackType &evalStack);
void evalMulNode(Graph &g, Node &n, EvalStackType &evalStack);
void evalDivNode(Graph &g, Node &n, EvalStackType &evalStack);
void evalNegateNode(Graph &g, Node &n, EvalStackType &evalStack);

void evalVec2CastNode(Graph &g, Node &n, EvalStackType &evalStack);
void evalVec3CastNode(Graph &g, Node &n, EvalStackType &evalStack);
void evalVec4CastNode(Graph &g, Node &n, EvalStackType &evalStack);
void evalMat3CastNode(Graph &g, Node &n, EvalStackType &evalStack);
void evalMat4CastNode(Graph &g, Node &n, EvalStackType &evalStack);

void evalLengthNode(Graph &g, Node &n, EvalStackType &evalStack);
void evalDistanceNode(Graph &g, Node &n, EvalStackType &evalStack);
void evalDotNode(Graph &g, Node &n, EvalStackType &evalStack);
void evalCrossNode(Graph &g, Node &n, EvalStackType &evalStack);
void evalNormalizeNode(Graph &g, Node &n, EvalStackType &evalStack);

void evalTransposeNode(Graph &g, Node &n, EvalStackType &evalStack);
void evalInverseNode(Graph &g, Node &n, EvalStackType &evalStack);
void evalDeterminantNode(Graph &g, Node &n, EvalStackType &evalStack);

void evalVec2CombineNode(Graph &g, Node &n, EvalStackType &evalStack);
void evalVec3CombineNode(Graph &g, Node &n, EvalStackType &evalStack);
void evalVec4CombineNode(Graph &g, Node &n, EvalStackType &evalStack);
void evalSwizzleNode(Graph &g, Node &n, EvalStackType &evalStack);

} // namespace

#endif
