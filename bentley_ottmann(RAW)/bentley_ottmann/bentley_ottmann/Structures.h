#pragma once
#include <math.h>
#include <string>
#include <iostream>
#include <vector>
using namespace std;

double GENERAL_EPSILON = 0.000000001;
double POINT_EPSILON = 0.000000001;

enum class Type { LEFT, RIGHT, INTERSECTION };


class Point {
private:
	double x;
	double y;

public:
	Point() {
		x = y = 0;
	}

	Point(double x1, double y1) {
		x = x1;
		y = y1;
	}

	double distance(Point p) {
		return sqrt(pow(this->x - p.x, 2) + pow(this->y - p.y, 2));
	}

	bool equals(Point other) {
		bool flat = fabs(this->x - other.x) < POINT_EPSILON && fabs(this->y - other.y) < POINT_EPSILON;
		return fabs(this->x - other.x) < POINT_EPSILON && fabs(this->y - other.y) < POINT_EPSILON;
	}

	double getX() {
		return x;
	}

	double getY() {
		return y;
	}

	void setX(double d) {
		x = d;
	}

	void setY(double d) {
		y = d;
	}

	string toString() {
		return "Point: (" + to_string(x) + ", " + to_string(y) + ")";
	}
};

class LineSegment {
private:
	Point p1;
	Point p2;

	// Finds the area of the triangle formed by three points using the shoelace formula.
	double area(Point a, Point b, Point c) {
		return 0.5 * (a.getX() * (b.getY() - c.getY()) + b.getX() * (c.getY() - a.getY())
			+ c.getX() * (a.getY() - b.getY()));
	}

	// Given two collinear line segments, determines if they intersect at more than
	// one point
	bool collinearIntersects(LineSegment* other) {
		/*
		 * NOTE: If two line segments share an endpoint, then they are adjacent to each other in the polygon and are
		 * thus NOT considered intersecting. This method checks if the line segments have any overlap at more than one
		 * point
		 */

		 // We will use the x-values of the endpoints to determine intersection (and
		 // y-values in the case where the line segments are vertical)
		if (fabs(this->p1.getX() - this->p2.getX()) < POINT_EPSILON)
		{
			// This is the case where the line segments are vertical. We take the
			// y-coordinates
			// of the top and bottom point of each line segment
			double thisTop, thisBottom;
			if (this->p1.getY() > this->p2.getY())
			{
				thisTop = this->p1.getY();
				thisBottom = this->p2.getY();
			}
			else
			{
				thisTop = this->p2.getY();
				thisBottom = this->p1.getY();
			}

			double otherTop, otherBottom;
			if (other->p1.getY() > other->p2.getY())
			{
				otherTop = other->p1.getY();
				otherBottom = other->p2.getY();
			}
			else
			{
				otherTop = other->p2.getY();
				otherBottom = other->p1.getY();
			}

			/*
			 * For the line segments to intersect, two conditions must hold: 1. The bottom point of this line segment
			 * must be below the top point of other line segment 2. The bottom point of other line segment must be below
			 * the top point of this line segment.
			 */
			if (thisBottom < otherTop && otherBottom < thisTop)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			// This case handles all cases where the collinear line segments are not
			// vertical.
			// We take the x coordinates of the left and right points of each line segment.
			double thisRight, thisLeft;
			if (this->p1.getX() > this->p2.getX())
			{
				thisRight = this->p1.getX();
				thisLeft = this->p2.getX();
			}
			else
			{
				thisRight = this->p2.getX();
				thisLeft = this->p1.getX();
			}

			double otherRight, otherLeft;
			if (other->p1.getX() > other->p2.getX())
			{
				otherRight = other->p1.getX();
				otherLeft = other->p2.getX();
			}
			else
			{
				otherRight = other->p2.getX();
				otherLeft = other->p1.getX();
			}

			/*
			 * For the line segments to intersect, two conditions must hold: 1. The left point of this line segment must
			 * be to the left of the right point of other line segment 2. The left point of other line segment must be
			 * to the left of the right point of this line segment
			 */
			if (thisLeft < otherRight && otherLeft < thisRight)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	// Checks if two line segments are collinear
	bool isCollinearTo(LineSegment* other){
		/*
		 * This method has two parts: 1. We check if the two endpoints of this line segment are collinear with p1 of
		 * other line segment, 2. We check if the two endpoints of this line segment are collinear with p2 of other line
		 * segment. To check if three points are collinear, we find the area of the triangle formed by the three points.
		 * If the area is zero, then the points are collinear
		 */

		return fabs(area(this->p1, this->p2, other->p1)) < POINT_EPSILON
			&& fabs(area(this->p1, this->p2, other->p2)) < POINT_EPSILON;
	}

	// Checks if an line segment straddles another (i.e. the directions from one
	// line segment to the two endpoints of the other line segment are opposites).
	bool straddles(LineSegment* other){
		/*
		 * We use three vectors in question: 1. The vector from the first point of "this" line segment to the first
		 * point of other line segment, 2. The vector from the first point of "this" line segment to the last point of
		 * other line segment, 3. This line segment we take the cross product of vector (1) and vector (3), and the
		 * cross product of vector (2) and vector (3). If they have the same sign (or if one of the cross products is
		 * 0), then this line segment does not "straddle" the other line segment.
		 */

		double orientationWithFirstPoint = crossProductK(this->p1, other->p1, this->p1, this->p2);
		double orientationWithLastPoint = crossProductK(this->p1, other->p2, this->p1, this->p2);

		if ((orientationWithFirstPoint < 0.0 && orientationWithLastPoint < 0.0)
			|| (orientationWithFirstPoint > 0.0 && orientationWithLastPoint > 0.0)
			|| fabs(orientationWithFirstPoint) < POINT_EPSILON
			|| fabs(orientationWithLastPoint) < POINT_EPSILON)
		{
			return false;
		}

		return true;
	}
	// Returns only the k-component of the cross product, not the entire orthogonal
	// vector.
	// In 2D space when extending the cross product to 3D space, the i and j
	// components are zero, and only the k-component is non-zero, making it useful
	// for determining clockwise or counterclockwise orientation.
	// For v1 x v2, if v1 is above v2 (counterclockwise) then the k-component is negative,
	// if v1 is below v2 (clockwise) then the k-component is positive, else it is 0.0 if v1 and v2 are parallel.
	double crossProductK(Point v1p1, Point v1p2, Point v2p1, Point v2p2){
		return (v1p2.getX() - v1p1.getX()) * (v2p2.getY() - v2p1.getY())
			- (v1p2.getY() - v1p1.getY()) * (v2p2.getX() - v2p1.getX());
	}

public:
	LineSegment(Point begin, Point end){
		this->p1 = begin;
		this->p2 = end;
	}
	LineSegment() {}
	bool isHorizontal(){
		return fabs(p1.getY() - p2.getY()) < POINT_EPSILON ? true : false;
	}

	bool isVertical(){
		return fabs(p1.getX() - p2.getX()) < POINT_EPSILON ? true : false;
	}

	double length(){
		return sqrt(pow(p1.getX() - p2.getX(), 2) + pow(p1.getY() - p2.getY(), 2));
	}
	Point getLeftEndpoint(){
		if (!isVertical())
		{
			return (p1.getX() < p2.getX()) ? p1 : p2;
		}
		else
		{
			return (p1.getY() < p2.getY()) ? p1 : p2;
		}
	}

	Point getRightEndpoint(){
		if (!isVertical())
		{
			return (p1.getX() > p2.getX()) ? p1 : p2;
		}
		else
		{
			return (p1.getY() > p2.getY()) ? p1 : p2;
		}
	}
	Point getP1(){
		return p1;
	}

	Point getP2(){
		return p2;
	}
	/*
	 * Checks for line segment intersection.
	 */
	bool intersects(LineSegment* other){
		/*
		 * There are two cases 1. The line segments are collinear. In this case, we check if the line segments intersect
		 * by comparing their x-values (if the right endpoint of the leftmost line segment is to the left of the left
		 * endpoint of the rightmost line segment, then the segments do not intersect) 2. The two line segments are not
		 * collinear. In this case, we need to check that both line segments straddle each other (i.e. the directions
		 * from one line segment to the two endpoints of the other line segment are opposites). NOTE: If the line
		 * segments intersect at exactly one endpoint, then this means that they are adjacent to each other in the
		 * polygon, and so for the purposes of this method, we will consider them not intersecting. In the case where
		 * two line segments share an endpoint.
		 */

		 // First we check if the line segments share an endpoint.
		if (this->p1.equals(other->p2) || this->p2.equals(other->p1) || this->p1.equals(other->p1)
			|| this->p2.equals(other->p2))
		{
			/*
			 * The only way for these two line segments to intersect (at any point besides the point they share) is for
			 * them to be collinear and have some overlap. Thus we check for collinearity (if their cross product is
			 * zero, then they are collinear)
			 */
			if (this->isCollinearTo(other))
			{
				return collinearIntersects(other);
			}
			else
			{
				// If the adjacent line segments are not collinear, then they cannot intersect
				return false;
			}
		}

		// The line segments do not share an endpoint. They are parallel if their cross
		// product is zero
		if (this->isCollinearTo(other))
		{
			return collinearIntersects(other);
		}
		else
		{
			// If the line segments are not collinear, we check if both line segments
			// straddle each other.
			// If so, then they intersect, otherwise, they do not.
			if (this->straddles(other) && other->straddles(this))
				return true;

			return false;
		}
	}

	/**
	 * Calculates the point of intersection between this and another line segment, but will return null if the line
	 * segments do not intersect or are collinear. Note that line segments that share just an endpoint are not
	 * considered intersecting.
	 *
	 * @param other The other line segment
	 * @return The point where this and the other line segment intersect.
	 */
	Point* getIntersectionPointWith(LineSegment* other){
		if (this->isCollinearTo(other) || !this->intersects(other))
		{
			return nullptr;
		}
		else
		{
			double x1 = p1.getX();
			double y1 = p1.getY();
			double x2 = other->p1.getX();
			double y2 = other->p1.getY();
			double m1, m2, b1, b2, x, y;

			if (!this->isVertical() && !other->isVertical())
			{
				m1 = (p2.getY() - p1.getY()) / (p2.getX() - p1.getX());
				m2 = (other->p2.getY() - other->p1.getY()) / (other->p2.getX() - other->p1.getX());
				b1 = y1 - m1 * x1;
				b2 = y2 - m2 * x2;

				x = (b2 - b1) / (m1 - m2);
				y = m1 * x + b1;
			}
			else if (this->isVertical())
			{
				m2 = (other->p2.getY() - other->p1.getY()) / (other->p2.getX() - other->p1.getX());
				b2 = y2 - m2 * x2;

				x = x1;
				y = m2 * x + b2;
			}
			else
			{
				m1 = (p2.getY() - p1.getY()) / (p2.getX() - p1.getX());
				b1 = y1 - m1 * x1;

				x = x2;
				y = m1 * x + b1;
			}

			Point* result = new Point(x, y);
			return result;
		}
	}

	int compareTo(LineSegment* other, Point eventPoint) {
		Point thisLeft = this->getLeftEndpoint();
		Point thisRight = this->getRightEndpoint();
		Point otherLeft = other->getLeftEndpoint();
		Point otherRight = other->getRightEndpoint();

		// Case where line segments are the same, i.e., line segment has been found.
		if (thisLeft.equals(otherLeft) && thisRight.equals(otherRight))
		{
			return 0;
		}
		// Case where line segments share an exact endpoint.
		else if (thisLeft.equals(otherLeft))
		{
			double orientation = crossProductK(thisLeft, thisRight, otherLeft, otherRight);

			if (orientation <= 0.0)
			{
				return 1;
			}
			else
			{
				return -1;
			}
		}
		// General case where line segments are compared at their event points with the sweep line.
		else
		{
			double x, y, x0, y0; // points
			double v1, v2; // vector components in the directions of the x and y axes respectively.

			// This assumes that both line segments cannot be vertical, i.e., two vertical line segments should not be
			// intersecting when found by the sweep line.
			if (!other->isVertical())
			{
				x = eventPoint.getX();
				v1 = otherRight.getX() - otherLeft.getX();
				v2 = otherRight.getY() - otherLeft.getY();
				x0 = otherLeft.getX();
				y0 = otherLeft.getY();
			}
			else
			{
				x = otherLeft.getX();
				v1 = thisRight.getX() - thisLeft.getX();
				v2 = thisRight.getY() - thisLeft.getY();
				x0 = thisLeft.getX();
				y0 = thisLeft.getY();
			}

			// Using parametric equation for lines, to obtain interior points (i.e., non-endpoints).
			double t = (x - x0) / v1;
			y = y0 + v2 * t;

			if (t < 0.0 || t > 1.0)
			{
				cerr << "Warning: Event point that is out of other line segment range is being used for comparisons, algorithm may not perform correctly.";
			}

			if (!other->isVertical())
			{
				if (fabs(eventPoint.getY() - y) < POINT_EPSILON)
				{
					Point* crossingPoint = this->getIntersectionPointWith(other);

					if (crossingPoint != nullptr  && crossingPoint->equals(eventPoint))
					{
						double orientation = crossProductK(*crossingPoint, thisRight, *crossingPoint, otherRight);

						if (orientation <= 0.0)
						{
							return -1;
						}
						else
						{
							return 1;
						}
					}
					else
					{
						double orientation = crossProductK(eventPoint, thisRight, eventPoint, otherRight);

						if (orientation <= 0.0)
						{
							return 1;
						}
						else
						{
							return -1;
						}
					}
				}
				else if (eventPoint.getY() > y)
				{
					return 1;
				}
				else
				{
					return -1;
				}
			}
			else
			{
				if (fabs(otherLeft.getY() - y) < POINT_EPSILON)
				{
					if (thisRight.getY() >= otherRight.getY())
					{
						return 1;
					}
					else
					{
						return -1;
					}
				}
				else if (otherLeft.getY() > y)
				{
					return -1;
				}
				else
				{
					return 1;
				}
			}
		}
	}

	string toString(){
		return "LineSegment: {" + p1.toString() + ", " + p2.toString() + "}";
	}
};

class Event {

private:
	Point eventPoint;
	LineSegment* segment;
	LineSegment* intersectionSegment;
	Type eventType;

public:
	Event(){}

	Event(Point eventPoint, LineSegment* segment, LineSegment* intersectionSegment, Type eventType) {
		this->eventPoint = eventPoint;
		this->segment = segment;
		this->intersectionSegment = intersectionSegment;
		this->eventType = eventType;
	}

	Event(Point eventPoint, LineSegment* segment, Type eventType) {
		this->eventPoint = eventPoint;
		this->segment = segment;
		this->eventType = eventType;

		this->intersectionSegment = nullptr;
	}

	Point getEventPoint() {
		return eventPoint;
	}

	Type getEventType() {
		return eventType;
	}

	LineSegment* getIntersectionSegment() {
		return intersectionSegment;
	}

	LineSegment* getSegment() {
		return segment;
	}

	void setEventPoint(Point eventPoint) {
		this->eventPoint = eventPoint;
	}

	void setEventType(Type eventType) {
		this->eventType = eventType;
	}

	void setIntersectionSegment(LineSegment* intersectionSegment)
	{
		this->intersectionSegment = intersectionSegment;
	}

	void setSegment(LineSegment* segment)
	{
		this->segment = segment;
	}

	//string toString() {
	//	
	//	return x;
	//}
};

class Node
{
private:
	LineSegment* segment; // Line segment held by this node
	Node* parent; // Link to parent node above
	Node* left; // Link to left child
	Node* right; // Link to right child
	Node* predecessor; // Link to inorder predecessor of this node.
	Node* successor; // Link to inorder successor of this node.

public:
	Node() {
		segment = nullptr;
		parent = left = right = nullptr;
		predecessor = successor = nullptr;
	}

	Node* getLeftChild() {
		return left;
	}

	Node* getParent() {
		return parent;
	}

	Node* getPredecessor() {
		return predecessor;
	}

	Node* getRightChild() {
		return right;
	}

	LineSegment* getSegment() {
		return segment;
	}

	Node* getSuccessor() {
		return successor;
	}

	void setLeftChild(Node* left) {
		this->left = left;
	}

	void setNode(LineSegment* segment, Node* parent, Node* left, Node* right, Node* predecessor, Node* successor) {
		this->segment = segment;
		this->parent = parent;
		this->left = left;
		this->right = right;
		this->predecessor = predecessor;
		this->successor = successor;
	}

	void setParent(Node* parent) {
		this->parent = parent;
	}

	void setPredecessor(Node* predecessor) {
		this->predecessor = predecessor;
	}

	void setRightChild(Node* right) {
		this->right = right;
	}

	void setSegment(LineSegment* segment) {
		this->segment = segment;
	}

	void setSuccessor(Node* successor) {
		this->successor = successor;
	}
};

class BinarySearchTree
{
private:
	Node* root; // Root of the bst, implemented as a dummy node.
	Node* findInorderPredecessorOf(Node* p) {
		if (p->getLeftChild() != nullptr)
		{
			// If left subtree exists, then predecessor is its maximum node.
			return findMaxNodeFrom(p->getLeftChild());
		}
		else
		{
			// If left subtree does not exist, then predecessor is found in ancestor node, unless this node is the min.
			// Climb up ancestors using parent link, until you find a node whose parent's left child is not equal to
			// it, that parent is the predecessor.
			Node* child = p;
			Node* parent = child->getParent();

			while (parent != nullptr && child == parent->getLeftChild())
			{
				child = parent;
				parent = parent->getParent();
			}

			return parent;
		}
	}
	Node* findInorderSuccessorOf(Node* p)
	{
		if (p->getRightChild() != nullptr)
		{
			// If right subtree exists, then successor is its minimum node.
			return findMinNodeFrom(p->getRightChild());
		}
		else
		{
			// If right subtree does not exist, then successor is found in ancestor node, unless this node is the max.
			// Climb up ancestors using parent link, until you find a node whose parent's right child is not equal to
			// it, that parent is the successor.
			Node* child = p;
			Node* parent = child->getParent();

			while (parent != nullptr && child == parent->getRightChild())
			{
				child = parent;
				parent = parent->getParent();
			}

			return parent;
		}
	}
	Node* findMaxNodeFrom(Node* p)
	{
		Node* current = p;

		while (current->getRightChild() != nullptr)
		{
			current = current->getRightChild();
		}

		return current;
	}
	Node* findMinNodeFrom(Node* p){
		Node* current = p;

		while (current->getLeftChild() != nullptr)
		{
			current = current->getLeftChild();
		}

		return current;
	}
	Node* findNode(LineSegment* s, Point eventPoint, Node* p)
	{
		if (p == nullptr)
		{
			return nullptr;
		}
		else if (s->compareTo(p->getSegment(), eventPoint) == 0)
		{
			return p;
		}
		else if (s->compareTo(p->getSegment(), eventPoint) == -1)
		{
			return findNode(s, eventPoint, p->getLeftChild());
		}
		else
		{
			return findNode(s, eventPoint, p->getRightChild());
		}
	}
	int getCount(Node* p){
		if (p != nullptr)
		{
			return 1 + getCount((*p).getLeftChild()) + getCount((*p).getRightChild());
		}
		else
		{
			return 0;
		}
	}
	int getCountOf(LineSegment* s, Point eventPoint, Node* p){
		if (p != nullptr)
		{
			if (s->compareTo(p->getSegment(), eventPoint) == 0)
			{
				return 1 + getCountOf(s, eventPoint, p->getRightChild());
			}
			else if (s->compareTo(p->getSegment(), eventPoint) == -1)
			{
				return getCountOf(s, eventPoint, p->getLeftChild());
			}
			else
			{
				return getCountOf(s, eventPoint, p->getRightChild());
			}
		}
		else
		{
			return 0;
		}
	}
	int getHeight(Node* p){
		if (p != nullptr)
		{
			return 1 + max(getHeight(p->getLeftChild()), getHeight(p->getRightChild()));
		}
		else
		{
			return -1;
		}
	}
	LineSegment* getMax(Node* p){
		if (p->getRightChild() == nullptr)
		{
			return p->getSegment();
		}
		else
		{
			return getMax(p->getRightChild());
		}
	}
	LineSegment* getMin(Node* p){
		if (p->getLeftChild() == nullptr)
		{
			return p->getSegment();
		}
		else
		{
			return getMin(p->getLeftChild());
		}
	}
	Node* insert(LineSegment* s, Point eventPoint, Node* p){
		if (s->compareTo(p->getSegment(), eventPoint) == -1) // If < current node, insert left.
		{
			if (p->getLeftChild() == nullptr)
			{
				Node* newChild = new Node();
				newChild->setNode(s, p, nullptr, nullptr, nullptr, nullptr);
				p->setLeftChild(newChild);
				newChild->setPredecessor(findInorderPredecessorOf(newChild));
				newChild->setSuccessor(findInorderSuccessorOf(newChild));

				// Update successors and predecessors after insertion of new node.
				if (newChild->getPredecessor() != nullptr)
				{
					newChild->getPredecessor()->setSuccessor(newChild);
				}

				if (newChild->getSuccessor() != nullptr)
				{
					newChild->getSuccessor()->setPredecessor(newChild);
				}

				return newChild;
			}
			else
			{
				return insert(s, eventPoint, p->getLeftChild());
			}
		}
		else // if >= current node, insert right.
		{
			if (p->getRightChild() == nullptr)
			{
				Node* newChild = new Node();
				newChild->setNode(s, p, nullptr, nullptr, nullptr, nullptr);
				p->setRightChild(newChild);
				newChild->setPredecessor(findInorderPredecessorOf(newChild));
				newChild->setSuccessor(findInorderSuccessorOf(newChild));

				// Update successors and predecessors after insertion of new node.
				if (newChild->getPredecessor() != nullptr)
				{
					newChild->getPredecessor()->setSuccessor(newChild);
				}

				if (newChild->getSuccessor() != nullptr)
				{
					newChild->getSuccessor()->setPredecessor(newChild);
				}

				return newChild;
			}
			else
			{
				return insert(s, eventPoint, p->getRightChild());
			}
		}
	}
	Node* remove(LineSegment* s, Point eventPoint, Node* p){
		// If tree is empty.
		if (p == nullptr)
		{
			return p;
		}
		else if (s->compareTo(p->getSegment(), eventPoint) == -1)
		{
			p->setLeftChild(remove(s, eventPoint, p->getLeftChild()));

			if (p->getLeftChild() != nullptr)
			{
				p->getLeftChild()->setParent(p);
			}
		}
		else if (s->compareTo(p->getSegment(), eventPoint) == 1)
		{
			p->setRightChild(remove(s, eventPoint, p->getRightChild()));

			if (p->getRightChild() != nullptr)
			{
				p->getRightChild()->setParent(p);
			}
		}
		else
		{ // If matching LineSegment is found.

			// Node with no child.
			if (p->getLeftChild() == nullptr && p->getRightChild() == nullptr)
			{
				return nullptr;
			}
			else if (p->getLeftChild() == nullptr) // Node with only a right child.
			{
				return p->getRightChild();
			}
			else if (p->getRightChild() == nullptr) // Node with only a left child.
			{
				return p->getLeftChild();
			}
			else // If node has both children, copy inorder successor contents to it, and remove successor.
			{
				Node* successor = findInorderSuccessorOf(p);

				// Copy contents of successor to node.
				p->setSegment(successor->getSegment());
				p->setPredecessor(successor->getPredecessor());
				p->setSuccessor(successor->getSuccessor());

				if (p->getPredecessor() != nullptr)
				{
					p->getPredecessor()->setSuccessor(p);
				}

				if (p->getSuccessor() != nullptr)
				{
					p->getSuccessor()->setPredecessor(p);
				}

				// Remove the inorder successor.
				p->setRightChild(remove(successor->getSegment(), eventPoint, p->getRightChild()));

				if (p->getRightChild() != nullptr)
				{
					p->getRightChild()->setParent(p);
				}
			}
		}

		return p;
	}

	bool search(LineSegment* s, Point eventPoint, Node* p){
		if (p == nullptr)
		{
			return false;
		}
		else if (s->compareTo(p->getSegment(), eventPoint) == 0)
		{
			return true;
		}
		else if (s->compareTo(p->getSegment(), eventPoint) == -1)
		{
			return search(s, eventPoint, p->getLeftChild());
		}
		else
		{
			return search(s, eventPoint, p->getRightChild());
		}
	}
public:
	BinarySearchTree(){
		root = new Node(); // Dummy node as the root
		root->setLeftChild(nullptr);
		root->setRightChild(nullptr);
		root->setSegment(nullptr);
	}

	Node* add(LineSegment* s, Point eventPoint){
		if (root->getLeftChild() == nullptr)
		{
			Node* first = new Node();
			(*first).setNode(s, nullptr, nullptr, nullptr, nullptr, nullptr);
			root->setLeftChild(first);

			return first;
		}
		else
		{
			return insert(s, eventPoint, root->getLeftChild());
		}
	}

	bool contains(LineSegment* s, Point eventPoint){
		return search(s, eventPoint, root->getLeftChild());
	}
	Node* findNode(LineSegment* s, Point eventPoint){
		return findNode(s, eventPoint, root->getLeftChild());
	}

	int getCount(){
		return getCount(root->getLeftChild());
	}
	int getCountOf(LineSegment* s, Point eventPoint){
		return getCountOf(s, eventPoint, root->getLeftChild());
	}

	int getHeight(){
		return getHeight(root->getLeftChild());
	}

	LineSegment* getMax()
	{
		return getMax(root->getLeftChild());
	}

	Node* getMaxNode(){
		return findMaxNodeFrom(root->getLeftChild());
	}

	LineSegment* getMin(){
		return getMin(root->getLeftChild());
	}

	Node* getMinNode(){
		return findMinNodeFrom(root->getLeftChild());
	}

	bool isEmpty(){
		return root->getLeftChild() == nullptr;
	}
	void remove(LineSegment* s, Point eventPoint)
	{
		// TODO Using findNode function might be redundant and unnecessary, test code without it.
		Node* p = findNode(s, eventPoint, root->getLeftChild());

		if (p != nullptr)
		{
			// Update successors and predecessors before deletion of node.
			if (p->getPredecessor() != nullptr)
			{
				p->getPredecessor()->setSuccessor(p->getSuccessor());
			}

			if (p->getSuccessor() != nullptr)
			{
				p->getSuccessor()->setPredecessor(p->getPredecessor());
			}

			root->setLeftChild(remove(s, eventPoint, root->getLeftChild()));

			if (root->getLeftChild() != nullptr)
			{
				root->getLeftChild()->setParent(nullptr);
			}
		}
	}
	void swapNodeInfo(Node* p, Node* q)
	{
		LineSegment* temp = p->getSegment();

		// Swapping p with q.
		p->setSegment(q->getSegment());

		// Swapping q with p.
		q->setSegment(temp);
	}
};

class EventQueue{
	/**
	 * Heapifies a subtree rooted with node index i, producing a min heap through Floyd's method which utilizes the
	 * sift-down technique. The building of the heap is done in an optimal manner, taking O(n) time overall for all
	 * event points. Note that the Event array passed to this method will be modified, according to each Event's event
	 * point, sorting the points from left to right on the x-axis.
	 *
	 * @param events array of Event objects containing event points, the array will be heapified.
	 * @param size   Given size of the heap.
	 * @param i      The root index of the subtree.
	 */
private:
	static void heapify(vector<Event*>& events, int size, int i){
		// By minimum we refer to the event points that have a smaller x, and thus should be higher on the binary heap.
		int minimum = i; // Initialize minimum as root of subtree.
		int left = 2 * i; // left child = 2*i
		int right = 2 * i + 1; // right child = 2*i + 1

		double minX = events[minimum]->getEventPoint().getX();

		if (left <= size)
		{
			double leftX = events[left]->getEventPoint().getX();

			// If the two event points have the same x, then the minimum will be the point with the minimum y.
			if (fabs(leftX - minX) < POINT_EPSILON)
			{
				double minY = events[minimum]->getEventPoint().getY();
				double leftY = events[left]->getEventPoint().getY();

				if (leftY < minY)
				{
					minimum = left;
				}
			}
			// Else if leftX is less than minX then the left event point is smaller.
			else if (leftX < minX)
			{
				minimum = left;
				minX = leftX;
			}
		}

		if (right <= size)
		{
			double rightX = events[right]->getEventPoint().getX();

			// If the two event points have the same x, then the minimum will be the point with the minimum y.
			if (fabs(rightX - minX) < POINT_EPSILON)
			{
				double minY = events[minimum]->getEventPoint().getY();
				double rightY = events[right]->getEventPoint().getY();

				if (rightY < minY)
				{
					minimum = right;
				}
			}
			// If rightX is less than minX then the right event point is smaller.
			else if (rightX < minX)
			{
				minimum = right;
				minX = rightX;
			}
		}

		// If the minimum event point is not the root of the subtree.
		if (minimum != i)
		{
			// Swap parent and left or right child.
			swap(events, i, minimum);

			// Recursively heapify the affected sub-tree.
			heapify(events, size, minimum);
		}
	}

	/**
	 * Swaps two elements of an array.
	 *
	 * @param events the array
	 * @param x      the index of the first element to be swapped.
	 * @param y      the index of the second element to be swapped.
	 */
	static void swap(vector<Event*>& events, int x, int y)
	{
		Event* swap = events[x];
		events[x] = events[y];
		events[y] = swap;
	}

	vector<Event*> events;

	int arraySize; // size of the array
	int length; // current number of elements in the array

public:
	EventQueue(vector<Event*> events){
		length = events.size();
		arraySize = length + 1;

		this->events = vector<Event*>(arraySize);
		for (int i = 1; i < arraySize; i++)
			this->events[i] = events[i - 1];


		// Build heap (rearranges array)
		// i starts at last non-leaf node, heapfiying by sift-down technique.
		for (int i = length / 2; i > 0; i--)
		{
			heapify(this->events, length, i);
		}
	}

	EventQueue(int arraySize){
		length = 0;
		this->arraySize = arraySize;

		events = vector<Event*>(this->arraySize, new Event());
	}

	void add(Event* e){
		int loc = ++length;

		events[0] = e;
		double eventX = e->getEventPoint().getX();
		while (eventX < events[loc / 2]->getEventPoint().getX())
		{
			events[loc] = events[loc / 2];
			loc /= 2;
		}

		// If the two event points have the same x, then bubble up the point with the minimum y.
		double eventY = e->getEventPoint().getY();
		while (fabs(eventX - events[loc / 2]->getEventPoint().getX()) < POINT_EPSILON
			&& eventY < events[loc / 2]->getEventPoint().getY())
		{
			events[loc] = events[loc / 2];
			loc /= 2;
		}

		events[loc] = e;
	}

	void deleteEventPoint(Point eventPoint){
		events[0] = nullptr;
		for (int i = 1; i <= length; i++)
		{
			if (events[i]->getEventPoint().equals(eventPoint))
			{
				for (int j = i; j <= length - 1; j++)
				{
					events[j] = events[j + 1];
				}
				for (int j = length; j < arraySize; j++)
				{
					events[j] = nullptr;
				}

				length--;

				// Rebuild heap (rearranges array)
				// i starts at last non-leaf node, heapfiying by sift-down technique.
				for (int j = length / 2; j > 0; j--)
				{
					heapify(events, length, j);
				}

				break;
			}
		}
	}

	bool isEmpty(){
		return (length == 0) ? true : false;
	}

	Event* min(){
		return events[1];
	}

	Event* removeMin()
	{
		Event* min = events[1];
		Event* lastEvent = events[length];
		length--;

		int parent = 1;
		while (parent * 2 <= length)
		{
			int child = parent * 2;

			double rightChildX = events[child + 1]->getEventPoint().getX();
			double leftChildX = events[child]->getEventPoint().getX();
			double rightChildY = events[child + 1]->getEventPoint().getY();
			double leftChildY = events[child]->getEventPoint().getY();

			if (child != length && (rightChildX < leftChildX
				|| (fabs(rightChildX - leftChildX) < POINT_EPSILON && rightChildY < leftChildY)))
			{
				child++;
			}

			double childX = events[child]->getEventPoint().getX();
			double childY = events[child]->getEventPoint().getY();

			if (childX < lastEvent->getEventPoint().getX()
				|| (fabs(childX - lastEvent->getEventPoint().getX()) < POINT_EPSILON
					&& childY < lastEvent->getEventPoint().getY()))
			{
				events[parent] = events[child];
			}
			else
			{
				break;
			}

			parent = child;
		}

		events[parent] = lastEvent;

		return min;
	}
};