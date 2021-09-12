#pragma once

struct Gui_Rect {
	int x, y, w, h;
	Gui_Rect(int x = 0, int y = 0, int w = 0, int h = 0);
	operator bool() const { return w > 0 && h > 0; }
};

inline Gui_Rect::Gui_Rect(int x, int y, int w, int h):
	x { x }, y { y }, w { w }, h { h }
{ }

bool operator==(const Gui_Rect &a, const Gui_Rect &b);

Gui_Rect intersect_rects(const Gui_Rect &a, const Gui_Rect &b);

#include <memory>
#include <vector>

class Gui_Panel {
	Gui_Rect frame_;
	std::vector<std::unique_ptr<Gui_Panel>> childs_;
public:
	Gui_Panel(const Gui_Rect &frame): frame_ { frame } { }
	virtual ~Gui_Panel() {}
	virtual void draw(const Gui_Rect &rect);
	const Gui_Rect &frame() const { return frame_; }
	void add_child(std::unique_ptr<Gui_Panel> &&child);
};

class Gui {
	int width_ { 80 };
	int height_ { 25 };
	Gui_Panel screen_ { Gui_Rect { 0, 0, width_, height_ } };
public:
	static void move_to(int x, int y);
	void redraw(const Gui_Rect &rect);
	void run();
};
