// bentley_ottmann.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Structures.h"
#include <vector>
using namespace std;

vector<Event*> events;
EventQueue* eq;
BinarySearchTree* sweepLine;
int tot;

void init(vector<LineSegment*> segments){
	int segmentCount = segments.size();
	events = vector<Event*>(segmentCount * 2);
	tot = 0;

	int j = 0;
	for (int i = 0; i < segmentCount; i++)
	{
		events[j] = new Event(segments[i]->getLeftEndpoint(), segments[i], Type::LEFT);
		events[j + 1] = new Event(segments[i]->getRightEndpoint(), segments[i], Type::RIGHT);
		j += 2;
	}

	eq = new EventQueue(events);
	sweepLine = new BinarySearchTree();
}

vector<Point> findIntersections(){
	vector<Point> intersections;

	while (!eq->isEmpty())
	{
		Event* event = eq->removeMin();

		if (event->getEventType() == Type::LEFT)
		{
			Node* current = sweepLine->add(event->getSegment(), event->getEventPoint());
			Node* above = current->getSuccessor();
			Node* below = current->getPredecessor();

			if (above != nullptr)
			{
				Point* crossingPoint = current->getSegment()->getIntersectionPointWith(above->getSegment());

				if (crossingPoint != nullptr)
				{
					eq->add(new Event(*crossingPoint, current->getSegment(), above->getSegment(), Type::INTERSECTION));
				}
			}

			if (below != nullptr)
			{
				Point* crossingPoint = current->getSegment()->getIntersectionPointWith(below->getSegment());

				if (crossingPoint != nullptr)
				{
					eq->add(new Event(*crossingPoint, current->getSegment(), below->getSegment(),
						Type::INTERSECTION));

				}
			}

			if (above != nullptr && below != nullptr)
			{
				Point* crossingPoint = above->getSegment()->getIntersectionPointWith(below->getSegment());

				if (crossingPoint != nullptr && crossingPoint->getX() > event->getEventPoint().getX())
				{
					eq->deleteEventPoint(*crossingPoint);
				}
			}

		}
		else if (event->getEventType() == Type::RIGHT)
		{
			// event.getSegment().setBoundaryColor(Color.red);
			// Tester.frame.repaint();

			Node* removed = sweepLine->findNode(event->getSegment(), event->getEventPoint());
			Node* above = removed->getSuccessor();
			Node* below = removed->getPredecessor();

			sweepLine->remove(event->getSegment(), event->getEventPoint());

			if (above != nullptr && below != nullptr)
			{
				Point* crossingPoint = above->getSegment()->getIntersectionPointWith(below->getSegment());

				if (crossingPoint != nullptr && crossingPoint->getX() > event->getEventPoint().getX())
				{
					eq->add(new Event(*crossingPoint, above->getSegment(), below->getSegment(),
						Type::INTERSECTION));

				}
			}
		}
		else
		{
			// Report the intersecting pair.
			++tot;
			intersections.push_back(event->getEventPoint());

			LineSegment* aboveSegment, *belowSegment;

			if (event->getSegment()->compareTo(event->getIntersectionSegment(), event->getEventPoint()) == 1)
			{
				aboveSegment = event->getSegment();
				belowSegment = event->getIntersectionSegment();
			}
			else
			{
				aboveSegment = event->getIntersectionSegment();
				belowSegment = event->getSegment();
			}

			Node* above = sweepLine->findNode(aboveSegment, event->getEventPoint());
			Node* below = sweepLine->findNode(belowSegment, event->getEventPoint());
			sweepLine->swapNodeInfo(above, below);

			Node* top = above->getSuccessor();
			Node* bottom = below->getPredecessor();

			if (top != nullptr)
			{
				Point* crossingPoint = above->getSegment()->getIntersectionPointWith(top->getSegment());

				if (crossingPoint != nullptr && crossingPoint->getX() > event->getEventPoint().getX())
				{
					eq->add(new Event(*crossingPoint, above->getSegment(), top->getSegment(), Type::INTERSECTION));

				}

				crossingPoint = below->getSegment()->getIntersectionPointWith(top->getSegment());

				if (crossingPoint != nullptr && crossingPoint->getX() > event->getEventPoint().getX())
				{
					eq->deleteEventPoint(*crossingPoint);
				}
			}

			if (bottom != nullptr)
			{
				Point* crossingPoint = below->getSegment()->getIntersectionPointWith(bottom->getSegment());

				if (crossingPoint != nullptr && crossingPoint->getX() > event->getEventPoint().getX())
				{
					eq->add(new Event(*crossingPoint, below->getSegment(), bottom->getSegment(),
						Type::INTERSECTION));

				}

				crossingPoint = above->getSegment()->getIntersectionPointWith(bottom->getSegment());

				if (crossingPoint != nullptr && crossingPoint->getX() > event->getEventPoint().getX())
				{
					eq->deleteEventPoint(*crossingPoint);
				}
			}
		}
	}
	cout << "Total intersections: " << tot;
	return intersections;
}

void setSegments(vector<LineSegment*> segments){
	int segmentCount = segments.size();
	events = vector<Event*>(segmentCount * 2);

	int j = 0;
	for (int i = 0; i < segmentCount; i++)
	{
		events[j] = new Event(segments[i]->getLeftEndpoint(), segments[i], Type::LEFT);
		events[j + 1] = new Event(segments[i]->getRightEndpoint(), segments[i], Type::RIGHT);
		j += 2;
	}

	eq = new EventQueue(events);
}

int main()
{
	vector<LineSegment*> segments;

	FILE* stream;
	freopen_s(&stream, "in.txt", "r", stdin);
	freopen_s(&stream, "out.txt", "w", stdout);

	int n;
	scanf_s("%d", &n);
	for (int i = 0; i < n; i++) {
		double x1, x2, y1, y2;
		scanf_s("%lf%lf%lf%lf", &x1, &y1, &x2, &y2);

		Point left = Point(x1, y1);
		Point right = Point(x2, y2);

		LineSegment* segment = new LineSegment(left, right);
		segments.push_back(segment);
	}

	init(segments);

	vector<Point> points = findIntersections();
}

