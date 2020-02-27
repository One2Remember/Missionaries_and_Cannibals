//
// Emmanuel Gallegos
// 2/12/2020
// Artificial Intelligence
// Missionaries and Cannibals
//
// Defines and runs an algorithm to find the solution to the missionaries
// and cannibals problem using a graph (represented as a vector of nodes
// each containing pointers to their parents) to represent both the frontier and
// the previously visited nodes. The algorithm implements a depth first graph
// search by treating the frontier vector as a stack. Each time the boat crosses
// the river, the cost is increased by 1.
//
// Example Graph DFS flow:
//  Generate root node, add to frontier, parent is nullptr, cost is 0
//  While goal not found and frontier is not empty
//    Release node from frontier, add to visited vector
//    From node, generate nodes to represent all states that could result from
//      possible actions (eg: if we only have two missionaries on left, along w/
//      the boat, we could generate M right and MM right). Have all generated
//      nodes point to their parent node to maintain graph structure.
//    Push all generated nodes that have not been visited and represent a valid
//      state (no missionaries being eaten) to back of frontier vector
//    Delete the invalid nodes to avoid memory leak
//    Pop last node from frontier vector
//    If node is goal, we may terminate loop
//  End While
//  At the end of the algorithm, if the solution has been found, it may be
//  printed by traversing up the goal node's parent's pointers recursively
//  until the root node is encountered.
//

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <thread>
#include <chrono>
#include <cstdlib>
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::ostream;
#define INIT_STATE {3,3,1}
#define THREE_INTS 3*sizeof(int)
#define INIT_ACT "START"
#define M_RIGHT "Move one missionary to the right"
#define MM_RIGHT "Move two missionaries to the right"
#define M_LEFT "Move one missionary to the left"
#define MM_LEFT "Move two missionaries to the left"
#define C_RIGHT "Move one cannibal to the right"
#define CC_RIGHT "Move two cannibals to the right"
#define C_LEFT "Move one cannibal to the left"
#define CC_LEFT "Move two cannibals to the left"
#define MC_RIGHT "Move one missionary and one cannibal to the right"
#define MC_LEFT "Move one missionary and one cannibal to the left"


// Node used to track all data of a state
struct Node
{
    Node * parent;  // holds parent node to maintain graph structure
    int cost;       // initially 0, contains total cost to achieve this state
    int state[3];   // {M,C,B} (eg. {3,3,1} -> 3 miss, 3 can, 1 boat on left)
    string action;  // description of action taken to get to this state
};

// vvvvvvvvvvv THIS FUNCTION IS WHERE THE SEARCH ALGORITHM LIVES vvvvvvvvvvvv
// vvvvvvvvvvv THIS FUNCTION IS WHERE THE SEARCH ALGORITHM LIVES vvvvvvvvvvvv

// returns solution (ptr to node w/goal state) or failure (nullptr)
Node * depthFirstSearch();

// ^^^^^^^^^^^ THIS FUNCTION IS WHERE THE SEARCH ALGORITHM LIVES ^^^^^^^^^^^^
// ^^^^^^^^^^^ THIS FUNCTION IS WHERE THE SEARCH ALGORITHM LIVES ^^^^^^^^^^^^

// expand node and add valid nodes to frontier
// first vector is explored, second is frontier
Node * expandNode( Node *, vector<Node *> &, vector<Node *> & );

// add node to frontier or delete if not valid
// first vector is explored, second is frontier
void processNode( Node *, vector<Node *> &, vector<Node *> & );

// returns if node ref passed contains goal state
bool isGoal( Node * );

// returns if node has been visited or is already in frontier
bool redundant( Node * n, vector<Node *> &v, vector<Node *> &f );

// returns if node contains legal state
bool isLegal( Node * );

// print nodes up parent in chain
void reversePrint( Node *, ostream & );

// takes a node and prints it's state
void printState( Node *, ostream & );

int main()
{
    // RETURN SOLUTION (ptr to node w/goal state) OR FAILURE (nullptr)
    Node * solution = depthFirstSearch();

    // PROCESS SOLUTION ( OR FAILURE )
    if( solution )  // if solution has been found
    {
        reversePrint( solution, cout );   // vertically traverse graph
        cout << "\nTotal Cost: " << solution->cost << endl; // print cost
    }
    else    // otherwise print no solution found
    {
        cout << "No solution found" << endl;
    }

    return 0;
}

// returns solution (ptr to node w/goal state) or failure (nullptr)
Node * depthFirstSearch()
{
    // DECLARE FRONTIER, EXPLORED, TEMP NODE PTR

    vector <Node *> frontier,   // list of nodes to explore (stack)
                    explored;   // list of nodes already visited
    Node * tn = nullptr;        // temp node ptr holder

    // GENERATE FIRST NODE HOLDING INITIAL STATE {3,3,0}

    tn = new Node{ nullptr, 0, INIT_STATE, INIT_ACT };  // generate first node

    // CHECK IF FIRST NODE IS SOLUTION

    if( isGoal( tn ) )
    {
        return tn;
    }

    // IF IT ISN'T, PUSH IT TO FRONTIER

    frontier.push_back( tn );   // push first node

    // LOOP UNTIL FRONTIER EXHAUSTED OR SOLUTION FOUND
    do
    {
        // POP DEEPEST NODE FROM FRONTIER
        tn = frontier.back();       // get last element from frontier
        frontier.pop_back();        // pop last element from frontier

        // PUSH NODE TO EXPLORED LIST
        explored.push_back( tn );   // add node to visited list

        // GENERATE CHILDREN AND IF THE SOLUTION IS GENERATED RETURN IT HERE
        // OTHERWISE ADD CHILDREN TO FRONTIER
        tn = expandNode( tn, explored, frontier );
        if( tn )
        {
            return tn;
        }
    } while( !frontier.empty() ); // while more nodes to process

    // IF FRONTIER EXHAUSTED AND NO SOLUTION, RETURN FAILURE
    return nullptr;
}

// takes a node from frontier, adds it to explored set, then expands the node,
// returning any found solution, otherwise adding all valid nodes to frontier
Node * expandNode(Node * tn, vector<Node *> &explored, vector<Node *> &frontier)
{
    Node * t1 = nullptr,    // holds the possible node resulting from moving M
         * t2 = nullptr,    // holds the possible node resulting from moving MM
         * t3 = nullptr,    // holds the possible node resulting from moving C
         * t4 = nullptr,    // holds the possible node resulting from moving CC
         * t5 = nullptr;    // holds the possible node resulting from moving MC

    // GENERATE ALL POSSIBLE NODES FROM CURRENT STATE, RETURN IF GOAL NODE
    // FOUND DURING GENERATION

    if( tn->state[2] )  // if boat is on left side
    {
        // 'one missionary right'
        if( tn->state[0] > 0 )  // if at least one missionary on left side
        {
            // generate 'one missionary right'
            t1 = new Node{ tn, tn->cost + 1, {}, M_RIGHT };
            memcpy( t1->state, tn->state, THREE_INTS ); // copy old state
            t1->state[0]--; // one fewer missionaries on left
            t1->state[2]--; // one fewer boat on left

            // IF GOAL GENERATED, RETURN IT HERE
            if( isGoal( t1 ) )
            {
                return t1;
            }
            // IF NODE IS ILLEGAL, DELETE IT
            if( !isLegal( t1 ) )
            {
                delete t1;
                t1 = nullptr;
            }
        }
        // 'two missionaries right'
        if( tn->state[0] > 1 )  // if at least two missionary on left side
        {
            // generate 'two missionaries right'
            t2 = new Node{ tn, tn->cost + 1, {}, MM_RIGHT };
            memcpy( t2->state, tn->state, THREE_INTS ); // copy old state
            t2->state[0]-= 2;   // two fewer missionaries on left
            t2->state[2]--;     // one fewer boat on left

            // IF GOAL GENERATED, RETURN IT HERE
            if( isGoal( t2 ) )
            {
                return t2;
            }
            // IF NODE IS ILLEGAL, DELETE IT
            if( !isLegal( t2 ) )
            {
                delete t2;
                t2 = nullptr;
            }
        }
        // 'one cannibal right'
        if( tn->state[1] > 0 )  // if at least one cannibal on left side
        {
            // generate 'one cannibal right'
            t3 = new Node{ tn, tn->cost + 1, {}, C_RIGHT };
            memcpy( t3->state, tn->state, THREE_INTS ); // copy old state
            t3->state[1]--; // one fewer cannibals on left
            t3->state[2]--; // one fewer boat on left

            // IF GOAL GENERATED, RETURN IT HERE
            if( isGoal( t3 ) )
            {
                return t3;
            }
            // IF NODE IS ILLEGAL, DELETE IT
            if( !isLegal( t3 ) )
            {
                delete t3;
                t3 = nullptr;
            }
        }
        // 'two cannibals right'
        if( tn->state[1] > 1 )  // if at least two cannibal on left side
        {
            // generate 'two cannibals right'
            t4 = new Node{ tn, tn->cost + 1, {}, CC_RIGHT };
            memcpy( t4->state, tn->state, THREE_INTS ); // copy old state
            t4->state[1]-= 2;   // two fewer cannibals on right
            t4->state[2]--;     // one fewer boat on left

            // IF GOAL GENERATED, RETURN IT HERE
            if( isGoal( t4 ) )
            {
                return t4;
            }
            // IF NODE IS ILLEGAL, DELETE IT
            if( !isLegal( t4 ) )
            {
                delete t4;
                t4 = nullptr;
            }
        }
        // 'one missionary and one cannibal right'
        if( tn->state[0] > 0 && tn->state[1] > 0 )  // if c & m on left side
        {
            // generate 'one cannibal, one missionary right'
            t5 = new Node{ tn, tn->cost + 1, {}, MC_RIGHT };
            memcpy( t5->state, tn->state, THREE_INTS ); // copy old state
            t5->state[0]--; // one fewer missionary on left
            t5->state[1]--; // one fewer cannibal on left
            t5->state[2]--; // one fewer boat on left

            // IF GOAL GENERATED, RETURN IT HERE
            if( isGoal( t5 ) )
            {
                return t5;
            }
            // IF NODE IS ILLEGAL, DELETE IT
            if( !isLegal( t5 ) )
            {
                delete t5;
                t5 = nullptr;
            }
        }
    }
    else    // boat is on right side
    {
        // 'one missionary left'
        if( tn->state[0] < 3 )  // if at least one missionary on right side
        {
            // generate 'one missionary left'
            t1 = new Node{ tn, tn->cost + 1, {}, M_LEFT };
            memcpy( t1->state, tn->state, THREE_INTS ); // copy old state
            t1->state[0]++; // one more missionary on left
            t1->state[2]++; // one more boat on left

            // IF GOAL GENERATED, RETURN IT HERE
            if( isGoal( t1 ) )
            {
                return t1;
            }
            // IF NODE IS ILLEGAL, DELETE IT
            if( !isLegal( t1 ) )
            {
                delete t1;
                t1 = nullptr;
            }
        }
        // 'two missionaries left'
        if( tn->state[0] < 2 )  // if at least two missionary on right side
        {
            // generate 'two missionary left'
            t2 = new Node{ tn, tn->cost + 1, {}, MM_LEFT };
            memcpy( t2->state, tn->state, THREE_INTS ); // copy old state
            t2->state[0]+= 2;   // two more missionaries on left
            t2->state[2]++;     // one more boat on left

            // IF GOAL GENERATED, RETURN IT HERE
            if( isGoal( t2 ) )
            {
                return t2;
            }
            // IF NODE IS ILLEGAL, DELETE IT
            if( !isLegal( t2 ) )
            {
                delete t2;
                t2 = nullptr;
            }
        }
        // 'one cannibal left'
        if( tn->state[1] < 3 )  // if at least one cannibal on right side
        {
            // generate 'one cannibal left'
            t3 = new Node{ tn, tn->cost + 1, {}, C_LEFT };
            memcpy( t3->state, tn->state, THREE_INTS ); // copy old state
            t3->state[1]++; // one more cannibal on left
            t3->state[2]++; // one more boat on left

            // IF GOAL GENERATED, RETURN IT HERE
            if( isGoal( t3 ) )
            {
                return t3;
            }
            // IF NODE IS ILLEGAL, DELETE IT
            if( !isLegal( t3 ) )
            {
                delete t3;
                t3 = nullptr;
            }
        }
        // 'two cannibals left'
        if( tn->state[1] < 2 )  // if at least two cannibal on right side
        {
            // generate 'two cannibal left'
            t4 = new Node{ tn, tn->cost + 1, {}, CC_LEFT };
            memcpy( t4->state, tn->state, THREE_INTS ); // copy old state
            t4->state[1]+= 2;   // two more cannibals on left
            t4->state[2]++;     // one more boat on left

            // IF GOAL GENERATED, RETURN IT HERE
            if( isGoal( t4 ) )
            {
                return t4;
            }
            // IF NODE IS ILLEGAL, DELETE IT
            if( !isLegal( t4 ) )
            {
                delete t4;
                t4 = nullptr;
            }
        }
        // 'one missionary and one cannibal left'
        if( tn->state[0] < 3 && tn->state[1] < 3 )  // 1m 1c on right
        {
            // generate 'one missionary one cannibal left'
            t5 = new Node{ tn, tn->cost + 1, {}, MC_LEFT };
            memcpy( t5->state, tn->state, THREE_INTS ); // copy old state
            t5->state[0]++; // one more missionary left
            t5->state[1]++; // one more cannibal left
            t5->state[2]++; // one more boat left

            // IF GOAL GENERATED, RETURN IT HERE
            if( isGoal( t5 ) )
            {
                return t5;
            }
            // IF NODE IS ILLEGAL, DELETE IT
            if( !isLegal( t5 ) )
            {
                delete t5;
                t5 = nullptr;
            }
        }
    }

    // ADD NODES NOT IN FRONTIER, EXPLORED (DELETE THE REST)

    processNode( t1, explored, frontier );
    processNode( t2, explored, frontier );
    processNode( t3, explored, frontier );
    processNode( t4, explored, frontier );
    processNode( t5, explored, frontier );

    // RETURN FAILURE SO DFS LOOP KNOWS SOLUTION NOT FOUND
    return nullptr;
}

// recursively prints node actions in reverse order along with state diagrams
// represents a vertical graph traversal from goal leaf node to root (init node)
void reversePrint( Node * n, ostream &out )
{
    if( !n )
    {
        return;
    }
    reversePrint( n->parent, out );
    out << endl << n->action << endl << endl;
    printState( n, out );
    std::chrono::seconds dura(1);
    std::this_thread::sleep_for( dura );
    system("CLS");

}
// add node to frontier if not visited and valid, otherwise delete it
void processNode( Node * n, vector<Node *> &vis, vector<Node *> &frontier )
{
    // if valid and unvisited and not already in frontier, then push node
    if( n && !redundant( n, vis, frontier ) )
    {
        frontier.push_back( n );
    }
    // otherwise delete n as it is redundant or illegal
    else if( n )
    {
        delete n;
    }
}

// returns if node ref passed contains goal state
bool isGoal( Node * n )
{
    return ! ( n->state[0] || n->state[1] || n->state[2] );
}

// returns if node is legal (no more cannibals than missionaries)
bool isLegal( Node * n )
{
    if( !n )
    {
        return false;
    }
    bool moreCannibalsOnLeft = n->state[0] > 0 && n->state[1] > n->state[0],
         moreCannibalsOnRight = n->state[0] < 3 && n->state[0] > n->state[1];
    return !( moreCannibalsOnLeft || moreCannibalsOnRight );
}

// returns if node has been visited previously or is already in frontier
bool redundant( Node * n, vector<Node *> &v, vector<Node *> &f )
{
    bool visited = false,
         inFrontier = false;
    unsigned int i = 0;
    // traverse vector of visited nodes and compare states, if state data
    // is identical, the node has been visited previously
    while( i < v.size() && !visited )
    {
        visited = n->state[0] == v[i]->state[0] && n->state[1] == v[i]->state[1]
         && n->state[2] == v[i]->state[2];
         i++;
    }
    i = 0;
    while( i < f.size() && !visited && !inFrontier )
    {
        inFrontier = n->state[0] == f[i]->state[0] && n->state[1] == f[i]->state[1]
         && n->state[2] == f[i]->state[2];
         i++;
    }
    return visited || inFrontier;
}

// takes a node and prints it's state neatly formatted
void printState( Node * s, ostream & out )
{
    out << "\t{     |     }\n\t{ ";
    switch( s->state[0] )
    {
    case 0:
        out << "    | MMM";
        break;
    case 1:
        out << "M   | MM ";
        break;
    case 2:
        out << "MM  | M  ";
        break;
    case 3:
        out << "MMM |    ";
        break;
    }
    out << " }\n\t{ ";
    switch( s->state[1] )
    {
    case 0:
        out << "    | CCC";
        break;
    case 1:
        out << "C   | CC ";
        break;
    case 2:
        out << "CC  | C  ";
        break;
    case 3:
        out << "CCC |    ";
        break;
    }
    out << " }\n\t{   ";
    if( s->state[2] )
    {
        out << "B |     }";
    }
    else
    {
        out << "  | B   }";
    }
    out << "\n\t{     |     }" << endl;
}
