#ifndef GRAPHEVAL_H
#define GRAPHEVAL_H

#include <vector>
#include "portdata.h"

struct Graph;
struct Node;

namespace GraphEval {

void update(Graph &g);

void evalConstantNode(Graph &g, Node &n, std::vector<PortData>& evalStack);

void evalPlusNode(Graph &g, Node &n, std::vector<PortData>& evalStack);
void evalMinusNode(Graph &g, Node &n, std::vector<PortData>& evalStack);
void evalMulNode(Graph &g, Node &n, std::vector<PortData>& evalStack);
void evalDivNode(Graph &g, Node &n, std::vector<PortData>& evalStack);
void evalNegateNode(Graph &g, Node &n, std::vector<PortData>& evalStack);

} // namespace

#endif
