#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <stack>
#include <set>
#include <queue>
#include <algorithm>
#include <ctime>
#include <deque>
#include <math.h>
#include <climits>
#include <cstring>
#include <fstream>
#include <iterator>

using namespace std;

//#define OUTPUT_GAME_DATA
//#define REDIRECT_CIN_FROM_FILE
//#define REDIRECT_COUT_TO_FILE

const string INPUT_FILE_NAME = "input.txt";
const string OUTPUT_FILE_NAME = "output.txt";

constexpr int INVALID_NODE_DEPTH = -1;
constexpr int TREE_ROOT_NODE_DEPTH = 1;
constexpr int ZERO_CHAR = '0';
constexpr int DIRECTIONS_COUNT = 8;
constexpr int BYTE_SIZE = 8;
constexpr int PAIR = 2;
constexpr int EDGE_WEIGHT = 1;
constexpr int MINDIJSKTRA_DIST = 0;
constexpr int TWO_GATEWAYS = 2;
constexpr int GAMEOVER_NODES_COUNT = 2;


enum class NodeFlags : uint8_t {
	MutipleGatewaysChildren		= 0b0000'0001,
	GateWay						= 0b0000'0010,
	OneGatewaysChild			= 0b0000'0100,
};

typedef int NodeId;
constexpr NodeId INVALID_ID = -1;

typedef vector<NodeId> MinDijsktraPath;

class Node {
public:
	Node();
	Node(NodeId);

	Node(
		NodeId id,
		int nodeDepth,
		NodeId parentId,
		bool rootNote,
		bool explored,
		bool inFrontier,
		int dijkstraMinDistance,
		MinDijsktraPath minDijsktraPath
	);

	~Node();

	NodeId getId() const {
		return id;
	}

	int getNodeDepth() const {
		return nodeDepth;
	}

	NodeId getParentId() const {
		return parentId;
	}

	bool getRootNode() const {
		return rootNote;
	}

	bool getExplored() const {
		return explored;
	}

	bool getInFrontier() const {
		return inFrontier;
	}

	int getDijkstraMinDistance() const {
		return dijkstraMinDistance;
	}

	MinDijsktraPath getMinDijsktraPath() const {
		return minDijsktraPath;
	}

	void setId(NodeId id) { this->id = id; }
	void setNodeDepth(int nodeDepth) { this->nodeDepth = nodeDepth; }
	void setParentId(NodeId parentId) { this->parentId = parentId; }
	void setRootNode(bool rootNote) { this->rootNote = rootNote; }
	void setExplored(bool explored) { this->explored = explored; }
	void setInFrontier(bool inFrontier) { this->inFrontier = inFrontier; }
	void setDijkstraMinDistance(int dijkstraMinDistance) { this->dijkstraMinDistance = dijkstraMinDistance; }
	void setMinDijsktraPath(const MinDijsktraPath& minDijsktraPath) { this->minDijsktraPath = minDijsktraPath; }
	void setFlag(const NodeFlags flag);
	void unsetFlag(const NodeFlags flag);
	void resetGatewayChildrenFlags();

	bool hasFlag(const NodeFlags flag) const;

private:
	NodeId id;
	int nodeDepth;
	NodeId parentId;
	bool rootNote;
	bool explored;
	bool inFrontier;
	uint8_t flags;

	/// Game specific members
	int dijkstraMinDistance;
	MinDijsktraPath minDijsktraPath;
};

//*************************************************************************************************************
//*************************************************************************************************************

Node::Node() :
	id(INVALID_ID),
	nodeDepth(INVALID_NODE_DEPTH),
	parentId(INVALID_ID),
	rootNote(false),
	explored(false),
	inFrontier(false),
	dijkstraMinDistance(INT_MAX),
	minDijsktraPath(),
	flags(0)
{

}

//*************************************************************************************************************
//*************************************************************************************************************

Node::Node(NodeId id) :
	id(id),
	nodeDepth(INVALID_NODE_DEPTH),
	parentId(INVALID_ID),
	rootNote(false),
	explored(false),
	inFrontier(false),
	dijkstraMinDistance(INT_MAX),
	minDijsktraPath(),
	flags(0)
{

}

//*************************************************************************************************************
//*************************************************************************************************************

Node::Node(
	NodeId id,
	int nodeDepth,
	NodeId parentId,
	bool rootNote,
	bool explored,
	bool inFrontier,
	int dijkstraMinDistance,
	MinDijsktraPath minDijsktraPath
) :
	id(id),
	nodeDepth(nodeDepth),
	parentId(parentId),
	rootNote(rootNote),
	explored(explored),
	inFrontier(inFrontier),
	dijkstraMinDistance(dijkstraMinDistance),
	minDijsktraPath(minDijsktraPath),
	flags(0)
{

}

//*************************************************************************************************************
//*************************************************************************************************************

Node::~Node() {

}

//*************************************************************************************************************
//*************************************************************************************************************

void Node::setFlag(const NodeFlags flag) {
	flags |= static_cast<uint8_t>(flag);
}

//*************************************************************************************************************
//*************************************************************************************************************

void Node::unsetFlag(const NodeFlags flag) {
	flags &= ~(static_cast<uint8_t>(flag));
}

//*************************************************************************************************************
//*************************************************************************************************************

void Node::resetGatewayChildrenFlags() {
	unsetFlag(NodeFlags::MutipleGatewaysChildren);
	unsetFlag(NodeFlags::OneGatewaysChild);
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Node::hasFlag(const NodeFlags flag) const {
	return (flags & static_cast<uint8_t>(flag));
}

//*************************************************************************************************************
//*************************************************************************************************************

typedef vector<NodeId> ChildrenList;
typedef map<NodeId, ChildrenList> GraphMap;
typedef map<NodeId, Node*> IdNodeMap;
typedef vector<NodeId> DijsktraFrontier;
typedef vector<NodeId> NodeStack;
typedef deque<NodeId> NodeQueue;
typedef set<NodeId> NodeSet;

class Graph {
public:
	Graph();
	Graph(int nodesCount, bool undirected, GraphMap graph);
	~Graph();

	int getNodesCount() const {
		return nodesCount;
	}

	bool getUndirectedget() const {
		return undirected;
	}

	GraphMap getGraph() const {
		return graph;
	}

	IdNodeMap getIdNodeMap() const {
		return idNodeMap;
	}

	Node* getNode(NodeId nodeId) const {
		return idNodeMap.at(nodeId);
	}

	void setNodesCount(int nodesCount) { this->nodesCount = nodesCount; }
	void setUndirected(bool undirected) { this->undirected = undirected; }
	void setGraph(GraphMap graph) { this->graph = graph; }
	void setIdNodeMap(IdNodeMap idNodeMap) { this->idNodeMap = idNodeMap; }

	void addEdge(NodeId parentId, NodeId childId);

	/// Adds edge wiht alredy created nodes
	void addEdge(Node* parent, Node* child);

	void deleteEdge(NodeId parentId, NodeId childId);

	/// Used when new child should be added to the graph
	void createNode(NodeId nodeId, int nodeDepth, NodeId parentId);

	void clear();
	bool nodeCreated(NodeId nodeId) const;
	void deleteAllNodes();
	vector<NodeId> treeRootsIds() const;

	void dfs(
		NodeId treeRootNodeId,
		NodeId& firstToDelete,
		NodeId& secondToDelete
	);

	void bfs(NodeId treeRootNodeId);
	int getMaxNodeDepth() const;
	bool edgeExists(NodeId parent, NodeId child) const;
	vector<NodeId> backtrack(NodeId from, NodeId to) const;
	NodeId getFirstNodeId() const;
	int depthOfTree(NodeId nodeId) const;
	int treeDiameter(NodeId nodeId) const;
	void dijsktra(NodeId nodeId);
	void graphResetAlgParams();
	NodeId popMinDistChildInForntier(DijsktraFrontier& frontier) const;

	/// Game specific members
	MinDijsktraPath findClosestFlaggedNode(const NodeFlags flag) const;
	NodeId getGatewayChild(const NodeId parent) const;
	void markMultipleGatewayNodes();
	void findDoubleLinkOnDeathPath(
		const NodeId nodeOneBeforeGateWay,
		NodeId& firstToDelete,
		NodeId& secondToDelete
	);

private:
	int nodesCount;
	bool undirected;
	GraphMap graph;

	/// Map used to store all nodes, used to check if node is already created
	/// and for easy accesss when deleteing memory pointed by each node
	IdNodeMap idNodeMap;

	/// Game specific members
};

//*************************************************************************************************************
//*************************************************************************************************************

Graph::Graph() :
	nodesCount(0),
	undirected(true),
	graph()
{

}

//*************************************************************************************************************
//*************************************************************************************************************

Graph::Graph(int nodesCount, bool undirected, GraphMap graph) :
	nodesCount(nodesCount),
	undirected(undirected),
	graph(graph)
{

}

//*************************************************************************************************************
//*************************************************************************************************************

Graph::~Graph() {
	deleteAllNodes();
	graph.clear();
}

//*************************************************************************************************************
//*************************************************************************************************************

void Graph::deleteAllNodes() {
	for (IdNodeMap::iterator it = idNodeMap.begin(); it != idNodeMap.end(); ++it) {
		Node* node = it->second;

		if (node) {
			delete node;
			node = NULL;
		}
	}

	idNodeMap.clear();
	nodesCount = 0;
}

//*************************************************************************************************************
//*************************************************************************************************************

vector<NodeId> Graph::treeRootsIds() const {
	vector<NodeId> res;

	for (IdNodeMap::const_iterator nodeIt = idNodeMap.begin(); nodeIt != idNodeMap.end(); ++nodeIt) {
		NodeId nodeId = nodeIt->first;

		bool isChildOfANode = false;

		for (GraphMap::const_iterator graphIt = graph.begin(); graphIt != graph.end(); ++graphIt) {
			ChildrenList childrenList = graphIt->second;
			if (find(childrenList.begin(), childrenList.end(), nodeId) != childrenList.end()) {
				isChildOfANode = true;
				break;
			}
		}

		if (!isChildOfANode) {
			res.push_back(nodeId);
		}
	}

	return res;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Graph::dfs(
	NodeId treeRootNodeId,
	NodeId& firstToDelete,
	NodeId& secondToDelete
) {
	getNode(treeRootNodeId)->setNodeDepth(TREE_ROOT_NODE_DEPTH);

	NodeStack frontier;

	frontier.push_back(treeRootNodeId);
	idNodeMap[treeRootNodeId]->setInFrontier(true);

	bool foundDoubleLink = false;
	while (!frontier.empty() && !foundDoubleLink) {
		NodeId state = frontier.back();
		frontier.pop_back();
		idNodeMap[treeRootNodeId]->setInFrontier(false);

		idNodeMap[state]->setExplored(true);

		ChildrenList* children = &graph[state];
		for (size_t childIdx = 0; childIdx < children->size(); ++childIdx) {
			NodeId childId = children->at(childIdx);

			if (!idNodeMap[childId]->hasFlag(NodeFlags::OneGatewaysChild) && !idNodeMap[childId]->hasFlag(NodeFlags::MutipleGatewaysChildren)) {
				continue;
			}
			else if (idNodeMap[childId]->hasFlag(NodeFlags::MutipleGatewaysChildren)) {
				firstToDelete = childId;
				secondToDelete = getGatewayChild(childId);
				foundDoubleLink = true;
				break;
			}

			bool nodeExplored = idNodeMap[childId]->getExplored();
			bool nodeInFrontier = idNodeMap[childId]->getInFrontier();
			if (!nodeExplored && !nodeInFrontier) {
				frontier.push_back(childId);
				idNodeMap[childId]->setInFrontier(true);

				//int parentDepth = idNodeMap[state]->getNodeDepth();
				//idNodeMap[childId]->setNodeDepth(parentDepth + 1);
				//idNodeMap[childId]->setParentId(state);
			}
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Graph::bfs(NodeId treeRootNodeId) {
	getNode(treeRootNodeId)->setNodeDepth(TREE_ROOT_NODE_DEPTH);

	NodeQueue frontier;

	frontier.push_back(treeRootNodeId);
	idNodeMap[treeRootNodeId]->setInFrontier(true);

	while (!frontier.empty()) {
		NodeId state = frontier.front();
		frontier.pop_front();
		idNodeMap[state]->setInFrontier(false);

		idNodeMap[state]->setExplored(true);

		ChildrenList* children = &graph[state];
		for (size_t childIdx = 0; childIdx < children->size(); ++childIdx) {
			NodeId childId = children->at(childIdx);

			bool nodeExplored = idNodeMap[childId]->getExplored();
			bool nodeInFrontier = idNodeMap[childId]->getInFrontier();
			if (!nodeExplored && !nodeInFrontier) {
				frontier.push_back(childId);

				int parentDepth = idNodeMap[state]->getNodeDepth();
				idNodeMap[childId]->setNodeDepth(parentDepth + 1);
				idNodeMap[childId]->setParentId(state);
			}
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

int Graph::getMaxNodeDepth() const {
	int maxNodeDepth = INVALID_NODE_DEPTH;

	for (IdNodeMap::const_iterator nodeIt = idNodeMap.begin(); nodeIt != idNodeMap.end(); ++nodeIt) {
		int nodeDepth = nodeIt->second->getNodeDepth();
		if (nodeDepth > maxNodeDepth) {
			maxNodeDepth = nodeDepth;
		}
	}

	return maxNodeDepth;
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Graph::edgeExists(NodeId parent, NodeId child) const {
	bool res = false;

	if (nodeCreated(parent) && nodeCreated(child)) {
		ChildrenList children = graph.at(parent); // TODO: copying do not copy use * for children
		res = find(children.begin(), children.end(), child) != children.end();
	}

	return res;
}

//*************************************************************************************************************
//*************************************************************************************************************

vector<NodeId> Graph::backtrack(NodeId from, NodeId to) const {
	vector<NodeId> path;

	while (from != to) {
		path.push_back(from);
		from = idNodeMap.at(from)->getParentId();
	}

	path.push_back(to);

	reverse(path.begin(), path.end());

	return path;
}

//*************************************************************************************************************
//*************************************************************************************************************

NodeId Graph::getFirstNodeId() const {
	return idNodeMap.begin()->first;
}

//*************************************************************************************************************
//*************************************************************************************************************

int Graph::depthOfTree(NodeId nodeId) const {
	if (idNodeMap.at(nodeId)->getRootNode()) {
		return 0;
	}

	int maxdepth = 0;

	for (ChildrenList::const_iterator nodeIt = graph.at(nodeId).begin(); nodeIt != graph.at(nodeId).end(); ++nodeIt) {
		maxdepth = max(maxdepth, depthOfTree(*nodeIt));
	}

	return maxdepth + 1;
}

//*************************************************************************************************************
//*************************************************************************************************************

int Graph::treeDiameter(NodeId nodeId) const {
	if (idNodeMap.at(nodeId)->getRootNode()) {
		return 0;
	}

	int max1 = 0, max2 = 0;
	for (ChildrenList::const_iterator nodeIt = graph.at(nodeId).begin(); nodeIt != graph.at(nodeId).end(); ++nodeIt) {
		int h = depthOfTree(*nodeIt);
		if (h > max1) {
			max2 = max1;
			max1 = h;
		}
		else if (h > max2) {
			max2 = h;
		}
	}

	int maxChildDia = 0;
	for (ChildrenList::const_iterator nodeIt = graph.at(nodeId).begin(); nodeIt != graph.at(nodeId).end(); ++nodeIt) {
		maxChildDia = max(maxChildDia, treeDiameter(*nodeIt));
	}

	return max(maxChildDia, max1 + max2 + 1);
}

//*************************************************************************************************************
//*************************************************************************************************************

void Graph::dijsktra(NodeId nodeId) {
	DijsktraFrontier frontier;

	idNodeMap[nodeId]->setDijkstraMinDistance(MINDIJSKTRA_DIST);
	idNodeMap[nodeId]->setMinDijsktraPath({ nodeId });

	frontier.push_back(nodeId);
	idNodeMap[nodeId]->setInFrontier(true);

	while (!frontier.empty()) {
		// map is sorted and this gets the min dist node
		NodeId currentNodeId = popMinDistChildInForntier(frontier);

		idNodeMap[currentNodeId]->setExplored(true);

		ChildrenList* children = &graph[currentNodeId];
		for (size_t childIdx = 0; childIdx < children->size(); ++childIdx) {
			NodeId childId = children->at(childIdx);

			bool nodeExplored = idNodeMap[childId]->getExplored();
			if (!nodeExplored) {
				int childDijMinDist = idNodeMap[childId]->getDijkstraMinDistance();
				int parentDijMinDist = idNodeMap[currentNodeId]->getDijkstraMinDistance();
				int parentChlidDist = parentDijMinDist + EDGE_WEIGHT;

				int minDist = childDijMinDist;
				MinDijsktraPath minPath = idNodeMap[childId]->getMinDijsktraPath();

				if (parentChlidDist < childDijMinDist) {
					minDist = parentChlidDist;
					minPath = idNodeMap[currentNodeId]->getMinDijsktraPath();
					minPath.push_back(childId);
				}

				idNodeMap[childId]->setMinDijsktraPath(minPath);
				idNodeMap[childId]->setDijkstraMinDistance(minDist);

				if (!idNodeMap[childId]->getInFrontier()) {
					frontier.push_back(childId);
					idNodeMap[childId]->setInFrontier(true);
				}
			}
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Graph::graphResetAlgParams() {
	for (IdNodeMap::iterator nodeIt = idNodeMap.begin(); nodeIt != idNodeMap.end(); ++nodeIt) {
		nodeIt->second->setExplored(false);
		nodeIt->second->setInFrontier(false);
		nodeIt->second->setDijkstraMinDistance(INT_MAX);
		nodeIt->second->setMinDijsktraPath(MinDijsktraPath());
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

NodeId Graph::popMinDistChildInForntier(DijsktraFrontier& frontier) const {
	int smallestDistance = INT_MAX;
	DijsktraFrontier::iterator smatDistanceIterator = frontier.begin();
	NodeId smallestDistNodeId = *smatDistanceIterator;

	for (DijsktraFrontier::iterator it = frontier.begin(); it != frontier.end(); ++it) {
		Node* node = idNodeMap.at(*it);

		if (node->getDijkstraMinDistance() < smallestDistance) {
			smallestDistance = node->getDijkstraMinDistance();
			smatDistanceIterator = it;
			smallestDistNodeId = node->getId();
		}
	}

	frontier.erase(smatDistanceIterator);

	return smallestDistNodeId;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Graph::addEdge(NodeId parentId, NodeId childId) {
	graph[parentId].push_back(childId);

	if (undirected) {
		graph[childId].push_back(parentId);
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Graph::addEdge(Node* parent, Node* child) {
	Node* nodes[PAIR] = { parent, child };

	for (int nodeIdx = 0; nodeIdx < PAIR; ++nodeIdx) {
		Node* node = nodes[nodeIdx];
		NodeId nodeId = node->getId();

		// May be an addNode(Node*) would be nice
		if (!nodeCreated(nodeId)) {
			idNodeMap[nodeId] = node;
			graph[nodeId];
		}
	}

	addEdge(parent->getId(), child->getId());
	child->setParentId(parent->getId());
}

//*************************************************************************************************************
//*************************************************************************************************************

void Graph::deleteEdge(NodeId parentId, NodeId childId) {
	ChildrenList& children = graph[parentId];

	for (size_t childIdx = 0; childIdx < children.size(); ++childIdx) {
		if (childId == children[childIdx]) {
			children.erase(children.begin() + childIdx); // Save to erase in loop, because breaking
			break;
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Graph::createNode(NodeId nodeId, int nodeDepth, NodeId parentId) {
	if (!nodeCreated(nodeId)) {
		Node* node = new Node(nodeId, nodeDepth, parentId, false, false, false, INT_MAX, MinDijsktraPath());
		idNodeMap[nodeId] = node;
		graph[nodeId];
		++nodesCount;
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Graph::clear() {
	deleteAllNodes();
	graph.clear();
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Graph::nodeCreated(NodeId nodeId) const {
	return idNodeMap.end() != idNodeMap.find(nodeId);
}

//*************************************************************************************************************
//*************************************************************************************************************

MinDijsktraPath Graph::findClosestFlaggedNode(const NodeFlags flag) const {
	int minDistToGateway = INT_MAX;

	MinDijsktraPath res;

	for (auto& node : idNodeMap) {
		Node* nodePtr = node.second;
		if (nodePtr->hasFlag(flag)) {
			int nodeDijsktraDist = nodePtr->getDijkstraMinDistance();
			if (nodeDijsktraDist < minDistToGateway) {
				minDistToGateway = nodeDijsktraDist;
				res = nodePtr->getMinDijsktraPath();
			}
		}
	}

	return res;
}

//*************************************************************************************************************
//*************************************************************************************************************

NodeId Graph::getGatewayChild(const NodeId parent) const {
	NodeId gatewayChild = INVALID_ID;
	const ChildrenList& children = graph.at(parent);

	for (const NodeId nodeId : children) {
		if (idNodeMap.at(nodeId)->hasFlag(NodeFlags::GateWay)) {
			gatewayChild = nodeId;
			break;
		}
	}

	return gatewayChild;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Graph::markMultipleGatewayNodes() {
	for (const pair<NodeId, Node*>& node : idNodeMap) {
		Node* nodePtr = node.second;
		if (!nodePtr->hasFlag(NodeFlags::GateWay)) {
			NodeId parentId = nodePtr->getId();
			const ChildrenList& children = graph[parentId];

			int gatewayChildrenCount = 0;

			for (NodeId childId : children) {
				Node* childNode = idNodeMap[childId];

				if (childNode->hasFlag(NodeFlags::GateWay)) {
					++gatewayChildrenCount;
				}
			}

			nodePtr->resetGatewayChildrenFlags();

			if (1 == gatewayChildrenCount) {
				nodePtr->setFlag(NodeFlags::OneGatewaysChild);
			}
			else if (gatewayChildrenCount >= TWO_GATEWAYS) {
				nodePtr->setFlag(NodeFlags::MutipleGatewaysChildren);
			}
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Graph::findDoubleLinkOnDeathPath(
	const NodeId nodeOneBeforeGateWay,
	NodeId& firstToDelete,
	NodeId& secondToDelete
) {
	graphResetAlgParams();
	dfs(nodeOneBeforeGateWay, firstToDelete, secondToDelete);
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

class Game {
public:
	Game();
	~Game();

	void initGame();
	void gameBegin();
	void gameLoop();
	void getGameInput();
	void getTurnInput();
	void turnBegin();
	void makeTurn();
	void turnEnd();
	void play();

	void debug() const;

	/// Game specific members

private:
	int turnsCount;

	/// Game specific members
	Graph skynetNetwork;
	NodeId agentNodeId;
};

//*************************************************************************************************************
//*************************************************************************************************************

Game::Game() :
	turnsCount(0),
	skynetNetwork(),
	agentNodeId(INVALID_ID)
{

}

//*************************************************************************************************************
//*************************************************************************************************************

Game::~Game() {
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::initGame() {
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::gameBegin() {
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::gameLoop() {
	while (true) {
		turnBegin();
		getTurnInput();
		makeTurn();
		turnEnd();
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::getGameInput() {
	int N; // the total number of nodes in the level, including the gateways
	int L; // the number of links
	int E; // the number of exit gateways
	cin >> N >> L >> E; cin.ignore();

#ifdef OUTPUT_GAME_DATA
	cerr << N << " " << L << " " << E << endl;
#endif

	for (int i = 0; i < L; i++) {
		int N1; // N1 and N2 defines a link between these nodes
		int N2;
		cin >> N1 >> N2; cin.ignore();

#ifdef OUTPUT_GAME_DATA
		cerr << N1 << " " << N2 << endl;
#endif

		// The graph handles the deallocation of the memory
		Node* n1Node = new Node(N1);
		Node* n2Node = new Node(N2);

		skynetNetwork.addEdge(n1Node, n2Node);
	}

	for (int i = 0; i < E; i++) {
		int EI; // the index of a gateway node
		cin >> EI; cin.ignore();

#ifdef OUTPUT_GAME_DATA
		cerr << EI << endl;
#endif

		skynetNetwork.getNode(EI)->setFlag(NodeFlags::GateWay);
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::getTurnInput() {
	int SI; // The index of the node on which the Skynet agent is positioned this turn
	cin >> SI; cin.ignore();

#ifdef OUTPUT_GAME_DATA
	cerr << SI << endl;
#endif

	agentNodeId = SI;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::turnBegin() {
	skynetNetwork.markMultipleGatewayNodes();
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::makeTurn() {
	skynetNetwork.dijsktra(agentNodeId);

	MinDijsktraPath minGatewayPath = skynetNetwork.findClosestFlaggedNode(NodeFlags::GateWay);
	MinDijsktraPath minDoubleGatewayPath = skynetNetwork.findClosestFlaggedNode(NodeFlags::MutipleGatewaysChildren);

	NodeId firstToDelete = minGatewayPath[minGatewayPath.size() - 2];
	NodeId secondToDelete = minGatewayPath.back();

	if (3 == minGatewayPath.size()) {
		const NodeId nodeOneBeforeGateWay = minGatewayPath[1];
		skynetNetwork.findDoubleLinkOnDeathPath(nodeOneBeforeGateWay, firstToDelete, secondToDelete);
	}
	else if (minDoubleGatewayPath.size() > 0 && minGatewayPath.size() > 2) {
		firstToDelete = minDoubleGatewayPath.back();
		secondToDelete = skynetNetwork.getGatewayChild(firstToDelete);
	}

	cout << firstToDelete << " " << secondToDelete << endl;

	skynetNetwork.deleteEdge(firstToDelete, secondToDelete);
	skynetNetwork.deleteEdge(secondToDelete, firstToDelete);
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::turnEnd() {
	++turnsCount;

	skynetNetwork.graphResetAlgParams();
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::play() {
	initGame();
	getGameInput();
	gameBegin();
	gameLoop();
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::debug() const {
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

#ifdef TESTS
#include "debug.h"
#endif // TESTS

int main(int argc, char** argv) {
#ifdef TESTS
	doctest::Context context;
	int res = context.run();
#else

#ifdef REDIRECT_CIN_FROM_FILE
	ifstream in(INPUT_FILE_NAME);
	streambuf *cinbuf = cin.rdbuf();
	cin.rdbuf(in.rdbuf());
#endif // REDIRECT_CIN_FROM_FILE

#ifdef REDIRECT_COUT_TO_FILE
	ofstream out(OUTPUT_FILE_NAME);
	streambuf *coutbuf = cout.rdbuf();
	cout.rdbuf(out.rdbuf());
#endif // REDIRECT_COUT_TO_FILE

	Game game;
	game.play();
#endif // TESTS

	return 0;
}