#include <vector>
#include <queue>
#include <set>
#include <list>
using namespace std;

class Point {
private:
	double x_coord;
	double y_coord;
public:
	Point () {}
	Point(double x, double y) {
		this->x_coord = x;
		this->y_coord = y;
	}

	double get_x_coord() {
		return this->x_coord;
	}

	void set_x_coord(double x_coord) {
		this->x_coord = x_coord;
	}

	double get_y_coord(){
		return this->y_coord;
	}

	void set_y_coord(double y_coord) {
		this->y_coord = y_coord;
	}
};

class Segment {
private:
	Point p_1;
	Point p_2;
	double value;

public:
	Segment(Point p_1, Point p_2) {
		this->p_1 = p_1;
		this->p_2 = p_2;
		this->calculate_value(this->first().get_x_coord());
	}

	Point first() {
		if (p_1.get_x_coord() <= p_2.get_x_coord()) {
			return p_1;
		}
		else {
			return p_2;
		}
	}

	Point second() {
		if (p_1.get_x_coord() <= p_2.get_x_coord()) {
			return p_2;
		}
		else {
			return p_1;
		}
	}

	void calculate_value(double value) {
		double x1 = this->first().get_x_coord();
		double x2 = this->second().get_x_coord();
		double y1 = this->first().get_y_coord();
		double y2 = this->second().get_y_coord();
		this->value = y1 + (((y2 - y1) / (x2 - x1)) * (value - x1));
	}

	void set_value(double value) {
		this->value = value;
	}

	double get_value() {
		return this->value;
	}
};

class Event {
private:
	Point point;
	vector<Segment*> segments;
	double value;
	int type;
public:
	Event(Point p, Segment* s, int type) {
		this->point = p;
		this->segments.push_back(s);
		this->value = p.get_x_coord();
		this->type = type;
	}
	Event(Point p, vector<Segment*> s, int type) {
		this->point = p;
		this->segments = s;
		this->value = p.get_x_coord();
		this->type = type;
	}
	void add_point(Point p) {
		this->point = p;
	}

	Point get_point() {
		return this->point;
	}

	void add_segment(Segment* s) {
		this->segments.push_back(s);
	}

	vector<Segment*> get_segments() {
		return this->segments;
	}

	void set_type(int type) {
		this->type = type;
	}

	int get_type() {
		return this->type;
	}

	void set_value(double value) {
		this->value = value;
	}

	double get_value() {
		return this->value;
	}
};

