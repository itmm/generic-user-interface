#include "gui.h"

#include <iostream>

bool operator==(const Gui_Rect &a, const Gui_Rect &b) {
	return a.x == b.x && a.y == b.y && a.w == b.w && a.h == b.h;
}

Gui_Rect intersect_rects(const Gui_Rect &a, const Gui_Rect &b) {
	int minx = std::min(a.x, b.x);
	int maxx = std::max(a.x, b.x);
	int miny = std::min(a.y, b.y);
	int maxy = std::max(a.y, b.y);
	int minw = std::min(a.w, b.w);
	int minh = std::min(a.h, b.h);
	return Gui_Rect {
		maxx, maxy, minw - maxx + minx, minh - maxy + miny
	};
}

std::ostream &operator<<(std::ostream &out, const Gui_Rect &r) {
	return out << '(' << r.x << ", " << r.y << ")x(" << 
		r.w << ", " << r.h << ')';
}

void assert_eq_rects(
	const char *file, int line, const Gui_Rect &a, const Gui_Rect &b
) {
	if (a == b) { } else {
		std::cerr << file << ':' << line <<
			" Gui_Rect does not match: " << a << " != " << b << '\n';
	}
}

#define ASSERT_EQ_RECTS(A, B) assert_eq_rects(__FILE__, __LINE__, (A), (B))

void assert_rect_empty(const char *file, int line, const Gui_Rect &r) {
	if (! r) { } else {
		std::cerr << file << ':' << line <<
			" Gui_Rect not empty: " << r << '\n';
	}
}

#define ASSERT_RECT_EMPTY(R) assert_rect_empty(__FILE__, __LINE__, (R))

void assert_rect_not_empty(const char *file, int line, const Gui_Rect &r) {
	if (r) { } else {
		std::cerr << file << ':' << line <<
			" Gui_Rect empty: " << r << '\n';
	}
}

#define ASSERT_RECT_NOT_EMPTY(R) assert_rect_not_empty(__FILE__, __LINE__, (R))

static inline void t_gui_rect() {
	ASSERT_RECT_EMPTY(Gui_Rect());
	ASSERT_RECT_NOT_EMPTY(Gui_Rect(1, 2, 4, 3));
	ASSERT_EQ_RECTS(Gui_Rect(1, 2, 4, 3), intersect_rects(
		Gui_Rect(1, 2, 4, 3), Gui_Rect(1, 2, 4, 3)
	));
	ASSERT_RECT_EMPTY(intersect_rects(Gui_Rect(1, 2, 4, 3), Gui_Rect()));
	ASSERT_RECT_EMPTY(intersect_rects(
		Gui_Rect(1, 2, 4, 3), Gui_Rect(5, 2, 4, 3)
	));
	ASSERT_EQ_RECTS(Gui_Rect(3, 3, 2, 2), intersect_rects(
		Gui_Rect(1, 2, 4, 3), Gui_Rect(3, 3, 4, 3)
	));
}

void Gui_Panel::draw(const Gui_Rect &rect) {
	if (intersect_rects(rect, frame_)) {
		for (auto &child : childs_) { child->draw(rect); }
	}
}

void Gui_Panel::add_child(std::unique_ptr<Gui_Panel> &&child) {
	childs_.push_back(std::move(child));
}

void Gui::move_to(int x, int y) {
	std::cout << "\x1b[" << (y + 1) << ';' << (x + 1) << 'H';
}

static void fill_rect(const Gui_Rect &r, char ch) {
	int je { r.y + r.h };
	int ie { r.x + r.w };
	for (int j { r.y}; j < je; ++j) {
		Gui::move_to(r.x, j);
		for (int i { r.x }; i < ie; ++i) { std::cout << ch; }
	}
}

void Gui::redraw(const Gui_Rect &rect) {
	auto r { intersect_rects(rect, screen_.frame()) };
	if (r == screen_.frame()) {
		std::cout << "\x1b[1J";
	} else {
		fill_rect(r, ' ');
	}
	screen_.draw(r);
}

class Test_Panel: public Gui_Panel {
public:
	Test_Panel(const Gui_Rect &frame): Gui_Panel { frame } { }
	void draw(const Gui_Rect &rect) {
		Gui_Rect fr { frame() };
		Gui_Rect content_frame { fr.x + 1, fr.y + 1, fr.w - 2, fr.h - 2 };
		auto r { intersect_rects(rect, content_frame ) };
		fill_rect(r, '.');
	}
};

void Gui::run() {
	t_gui_rect();
	screen_.add_child(std::make_unique<Test_Panel>(screen_.frame()));
	redraw(screen_.frame());

	std::cout << "done\n";
}
