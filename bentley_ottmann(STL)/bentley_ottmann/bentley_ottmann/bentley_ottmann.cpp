
#include "Structures.h"
#include <queue>
#include <set>
#include <iostream>
#include <map>
#include <chrono>
#include <list>
using namespace std;

double MinNum = 0.0000001;

auto event_comparator = [](Event* e_1, Event* e_2) {
	if (e_1->get_value() > e_2->get_value()) {
		return true;
	}
	return false;
};
priority_queue<Event*, vector<Event*>, decltype(event_comparator)> Q(event_comparator);

auto segment_comparator = [](Segment* s_1, Segment* s_2) {
	if (s_1->get_value() > s_2->get_value()) {
		return true;
	}
	return false;
};

set<Segment*, decltype(segment_comparator)> T(segment_comparator);

vector<Point> X;

vector<Point> tempP;


void init(vector<Segment*> input_data) {
	for (Segment* s : input_data) {
		Q.push(new Event(s->first(), s, 0));
		Q.push(new Event(s->second(), s, 1));

		tempP.push_back(s->first());
		tempP.push_back(s->second());
	}
}

bool report_intersection(Segment* s_1, Segment* s_2, double L) {
	double x1 = s_1->first().get_x_coord();
	double y1 = s_1->first().get_y_coord();
	double x2 = s_1->second().get_x_coord();
	double y2 = s_1->second().get_y_coord();
	double x3 = s_2->first().get_x_coord();
	double y3 = s_2->first().get_y_coord();
	double x4 = s_2->second().get_x_coord();
	double y4 = s_2->second().get_y_coord();
	double r = (x2 - x1) * (y4 - y3) - (y2 - y1) * (x4 - x3);
	if (r != 0) {
		double t = ((x3 - x1) * (y4 - y3) - (y3 - y1) * (x4 - x3)) / r;
		double u = ((x3 - x1) * (y2 - y1) - (y3 - y1) * (x2 - x1)) / r;
		if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
			double x_c = x1 + t * (x2 - x1);
			double y_c = y1 + t * (y2 - y1);
			if (x_c > L) {
				vector<Segment*> v_seg{ s_1, s_2 };

				Point p = Point(x_c, y_c);

				auto is_p = [x_c, y_c](Point pt) {
					return (fabs(pt.get_x_coord() - x_c) < MinNum && fabs(pt.get_y_coord() - y_c) < MinNum);
				};

				auto it = find_if(begin(tempP), end(tempP), is_p);

				if (it != end(tempP)) 
					return false;

				Q.push(new Event(p, v_seg, 2));
				tempP.push_back(p);

				return true;
			}
		}
	}
	return false;
}

void swap(Segment* s_1, Segment* s_2) {
	T.erase(s_1);
	T.erase(s_2);
	double value = s_1->get_value();
	s_1->set_value(s_2->get_value());
	s_2->set_value(value);
	T.insert(s_1);
	T.insert(s_2);
}

void recalculate(double L) {
	set<Segment*, decltype(segment_comparator)> ::iterator it = T.begin();
	while (it != T.end()) {
		(*it)->calculate_value(L);
		it++;
	}
}

void print_intersections() {
	for (Point p : X) {
		cout << "(" << p.get_x_coord() << ", " << p.get_y_coord() << ")" << endl;
	}
}

vector<Point> get_intersections() {
	return X;
}


void find_intersections() {
	while (!Q.empty()) {
		Event* e = Q.top();
		Q.pop();

		Point p = e->get_point();
		double px = p.get_x_coord();
		double py = p.get_y_coord();
		auto is_p = [px, py](Point pt) {
			return (fabs(pt.get_x_coord() - px) < MinNum && fabs(pt.get_y_coord() - py) < MinNum);
		};
		auto it = find_if(begin(tempP), end(tempP), is_p);
		tempP.erase(it);

		double L = e->get_value();
		switch (e->get_type())
		{
		case 0:
			for (Segment* s : e->get_segments()) {
				recalculate(L);
				T.insert(s);
				if (--T.lower_bound(s) != T.end()) {
					Segment* r = *--T.lower_bound(s);
					report_intersection(r, s, L);
				}
				if (T.upper_bound(s) != T.end()) {
					Segment* t = *T.upper_bound(s);
					report_intersection(t, s, L);
				}
				if (--T.lower_bound(s) != T.end() && T.upper_bound(s) != T.end()) {
					Segment* r = *--T.lower_bound(s);
					Segment* t = *T.upper_bound(s);
				}
			}
			break;
		case 1:
			for (Segment* s : e->get_segments()) {
				if (--T.lower_bound(s) != T.end() && T.upper_bound(s) != T.end()) {
					Segment* r = *--T.lower_bound(s);
					Segment* t = *T.upper_bound(s);
					report_intersection(r, t, L);
				}
				T.erase(s);
			}
			break;
		case 2:
			Segment * s_1 = e->get_segments()[0];
			Segment* s_2 = e->get_segments()[1];
			swap(s_1, s_2);
			if (s_1->get_value() < s_2->get_value()) {
				if (T.upper_bound(s_1) != T.end()) {
					Segment* t = *T.upper_bound(s_1);
					report_intersection(t, s_1, L);
				}
				if (--T.lower_bound(s_2) != T.end()) {
					Segment* r = *--T.lower_bound(s_2);
					report_intersection(r, s_2, L);
				}
			}
			else {
				if (T.upper_bound(s_2) != T.end()) {
					Segment* t = *T.upper_bound(s_2);
					report_intersection(t, s_2, L);
				}
				if (--T.lower_bound(s_1) != T.end()) {
					Segment* r = *--T.lower_bound(s_1);
					report_intersection(r, s_1, L);
				}
			}
			X.push_back(e->get_point());
			break;
		}
	}
}


int main() {

	vector<Segment*> segments;

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

		Segment* segment = new Segment(left, right);
		segments.push_back(segment);
	}

	init(segments);

	auto start = std::chrono::high_resolution_clock::now();
	find_intersections();
	auto end = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
	cout << "Total intersections: " << X.size() << endl;
	cout << "Duration: " << duration.count() << endl;

	print_intersections();
}

