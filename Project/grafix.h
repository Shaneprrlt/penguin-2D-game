//If you wish to write a GrafiX program, then close this file and open MyCode.cpp instead.

//******************************************************************************
//**************************GrafiX.h VERSION 4.5.05*****************************
//For use with MS Visual Studio 2008 Express
//******************************************************************************
/*
	GrafiX is a framework for doing graphics and animation using standard C++
	within a Windows environment.  It was developed by Dr. Erlan Wheeler of 
	Carthage College. (ewheeler.carthage.edu).  Visit the GrafiX site at
	http://www.carthage/ewheeler/grafix to learn more.
*/
//Notes
	//With Express 9, webdings over 126 were not printing.  The fix was to go to
	//Project->Props->Config->Gen->CharSet and remove Unicode.

#ifndef GRAFIX_H
#define GRAFIX_H

#include <windows.h>
#include <string>
#include <vector>
using std::string;
using std::vector;

//******************************************************************************
//**********************THE Color CLASS*****************************************
//******************************************************************************
class Color
{
public:
//Data Members
	int r, g, b;
    
public:
//Constructors
	Color(){r = 255; g = 255, b = 255;}
	Color(int red, int green, int blue);

//Methods
	//These two functions return a darkened/lightened copy of a Color.
	//Given Color c(100, 200, 255), then c.darken(50) returns a Color    
	//that is 50% of the way from c to BLACK, or r=50, g = 100, b = 127.

	Color dark (int percent = 50);	//returns a color darkened  by 0-100 percent
	Color light(int percent = 50);	//returns a color lightened by 0-100 percent

	Color inverse();	//returns the inverse of the Color
};

bool operator==(Color& lhs, Color& rhs);
bool operator!=(Color& lhs, Color& rhs);

//The 16 HTML colors
#define	AQUA		Color(  0, 255, 255)	//also CYAN
#define	BLACK		Color(  0,   0,   0)
#define	BLUE		Color(  0,   0, 255)
#define	FUCHSIA		Color(255,   0, 255)	//also MAGENTA
#define	GRAY		Color(128, 128, 128)	//also GREY, a dark gray
#define	GREEN		Color(  0, 128,   0)	//!!! a dark green
#define	LIME		Color(  0, 255,   0)
#define	MAROON		Color(128,   0,   0)	//also DARK_RED
#define	NAVY		Color(  0,   0, 128)	//also DARK_BLUE
#define	OLIVE		Color(128, 128,   0)	//also DARK_YELLOW
#define	PURPLE		Color(128,   0, 128)	//also DARK_MAGENTA
#define	RED			Color(255,   0,   0)
#define	SILVER		Color(192, 192, 192)	//also LIGHT_GRAY
#define	TEAL		Color(  0, 128, 128)	//a dark cyan
#define	WHITE		Color(255, 255, 255)
#define	YELLOW		Color(255,  255,  0)

//"Browser-safe" colors use hex 33(51), 66(102), 99(153), CC(204), & FF(255)
#define BRICK_RED	Color(204,   0,   0)
#define BROWN		Color(102,  51,   0)
#define CHOCOLATE	Color(204, 102,   0)
#define CYAN		Color(  0, 255, 255)	//also AQUA
#define FLESH		Color(255, 204, 153)
#define GOLD		Color(204, 153,   0)
#define GREY		Color(128, 128, 128)	//also GRAY
#define OLIVE_DRAB	Color(153, 153, 102)
#define ORANGE		Color(255, 102,   0)
#define MAGENTA		Color(255,   0, 255)	//also FUCHSIA
#define PINK		Color(255,   0, 102)
#define ROSE		Color(255,   0, 153)
#define SALMON		Color(255, 153, 153)
#define SKY_BLUE	Color( 51, 153, 255)
#define TAN			Color(255, 204, 102)
#define VIOLET		Color(102,   0, 255)

//Other Colors in the Windows minimal 16 color palette
#define DARK_BLUE	Color(  0,   0, 128)	//also NAVY
#define DARK_RED	Color(128,   0,   0)	//also MAROON
#define DARK_MAGENTA Color(128,   0,  0)	//also PURPLE
#define DARK_YELLOW Color(128, 128,   0)	//also OLIVE
#define LIGHT_GRAY	Color(192, 192, 192)	//also SILVER

//Other Colors in the Windows reserved 20 colors
#define MONEY_GREEN		Color(192, 220, 192)
#define LIGHT_SKY_BLUE	Color(166, 202, 240)
#define CREAM			Color(255, 251, 240)
#define MEDIUM_GRAY		Color(160, 160, 164)

//OTHERS

//******************************************************************************
enum {BUTTON_ABORT_RETRY_IGNORE, BUTTONS_ABORT_RETRY_IGNORE,
	  BUTTON_OK, BUTTONS_OK,
	  BUTTON_OK_CANCEL, BUTTONS_OK_CANCEL,
	  BUTTON_RETRY_CANCEL, BUTTONS_RETRY_CANCEL,
      BUTTON_YES_NO, BUTTONS_YES_NO,
	  BUTTON_YES_NO_CANCEL, BUTTONS_YES_NO_CANCEL,

	  ICON_EXCLAMATION, ICON_INFORMATION, ICON_QUESTION, ICON_STOP,

	  DEFAULT_BUTTON1, DEFAULT_BUTTON2, DEFAULT_BUTTON3, DEFAULT_BUTTON4,
	  
	  BUTTON_ABORT, BUTTON_CANCEL, BUTTON_IGNORE, BUTTON_NO, BUTTON_RETRY, BUTTON_YES,
      DEFAULT_MB};



//Pen styles - see set_pen_style()
enum {SOLID_PEN, DOT_PEN, DASH_PEN, DASH_DOT_PEN, DASH_DOT_DOT_PEN};	//Pen styles

///Text fonts - see set_text_font()
//Note that we can use other fonts on the system, e.g. "Edwardian Script ITC"
#define COURIER				"Courier New"
#define TIMES				"Times New Roman"
#define ARIAL				"Arial"
#define COURIER_BOLD		"Courier New Bold"
#define TIMES_BOLD			"Times New Roman Bold"
#define ARIAL_BOLD			"Arial Bold"
#define COURIER_ITALIC		"Courier New Italic"
#define TIMES_ITALIC		"Times New Roman Italic"
#define ARIAL_ITALIC		"Arial Italic"
#define COURIER_BOLD_ITALIC	"Courier New Bold Italic"
#define TIMES_BOLD_ITALIC	"Times New Roman Bold Italic"
#define ARIAL_BOLD_ITALIC	"Arial Bold Italic"
#define SYMBOL				"Symbol"

//Text alignments - see set_text_alignment()
enum {CENTER, LEFT_TOP, CENTER_TOP, RIGHT_TOP, LEFT_BASELINE, CENTER_BASELINE,
	  RIGHT_BASELINE, LEFT_BOTTOM, CENTER_BOTTOM, RIGHT_BOTTOM};

//The following are named pages.

enum {SCREEN = 0, BUFFER = 1, BUFFER2 = 2};

//Forward Declarations
class Animator;
class Color;
class Point;


//*****************************************************************************
//*************************THE Point CLASS*************************************
//*****************************************************************************

/*The Point class holds the coordinates of a screen point.  The screen is
coordinatized with (0,0) in the upper-left corner.  The botttom-right corner
is (MaxX, MaxY).  The middle of the screen is (MidX, MidY).  The function
get_click() returns a Point at which a mouse click occurs.
There are several special named Points as well.
*/

class Point
{
public:
	Point();
	Point(int x, int y);

	bool operator==(Point rhs);
	bool operator!=(Point rhs);

	Point operator+(Point rhs);
	Point operator-(Point rhs);

	Point operator/(int rhs);

	int x, y;
};

//MACROS FOR POSITIONS USING COORDINATES OR POINTS/////////

//Largest screen coordinates
int screen_width(); //forward declaration
int screen_height();//forward declaration
	#define MaxX		(screen_width()  - 1)
	#define MaxY		(screen_height() - 1)
	#define MaxPt		Point(MaxX, MaxY)

//Middle of the screen

	#define MidX		(screen_width()  / 2)
	#define MidY		(screen_height() / 2)
	#define MidPt		Point(MidX,MidY)

//Smallest screen coordinates

	#define MinX		0
	#define MinY		0
	#define MinPt		Point(0,0)
	
//Percentage p (from 0 - 100) across the screen

	#define PerX(percent)		((percent) * MaxX / 100)
	#define PerY(percent)		((percent) * MaxY / 100)
	#define PerPt(percent1, percent2)	Point(PerX(percent1), PerY(percent2))

//Random coordinates and Points on the screen

	#define RanX		random_x()
	#define RanY		random_y()
	#define RanPt		random_point()

//Current coordinates and Point of the cursor

	#define CurX		cursor_x()
	#define CurY		cursor_y()
	#define CurPt		cursor_point()

//Present coordinates of the vanishing Point (3D property)

	#define VanPt		get_3D_vanishing_point()

//L = left, R = right, T = top, B = bottom

	#define LMinPt		MinPt
	#define LMidPt		PerPt(0,50)
	#define LMaxPt		PerPt(0,100)
	#define RMinPt		PerPt(100,0)
	#define RMidPt		PerPt(100,50)
	#define RMaxPt		MaxPt
	#define TMinPt		MinPt
	#define TMidPt		PerPt(50,0)
	#define TMaxPt		PerPt(100,0)
	#define BMinPt		PerPt(0,100)
	#define BMidPt		PerPt(50,100)
	#define BMaxPt		MaxPt

	#define LTMidPt		PerPt(25,25)
	#define RTMidPt		PerPt(75,25)
	#define LBMidPt		PerPt(25,75)
	#define RBMidPt		PerPt(75,75)

//C = cell, the row = 1...r by col = 1..c cell in an r by c table
	#define CMidPt(row, col, r, c)	PerPt((100.0 * (col) - 50.0) / (c), (100 * (row) - 50.0) / (r))
	#define CLTPt(row, col, r, c)   PerPt((100.0 * ((col) - 1)) / (c) , 100.0 * ((row) - 1.0) / (r))
	#define CRTPt(row, col, r, c)	PerPt((100.0 * (col)) / (c), 100.0 * ((row) - 1) / (r))
	#define CLBPt(row, col, r, c)	PerPt((100.0 * ((col) - 1)) / (c), 100.0 * (row) / (r))
	#define CRBPt(row, col, r, c)	PerPt((100.0 * (col)) / (c), 100.0 * (row) / (r))
//*****************************************************************************
//*************************THE Animator CLASS**********************************
//*****************************************************************************
class Animator
{
public:
	static Animator* current;

public:
	Animator(double total_secs = 3600, bool repeat = false);

	bool animate();
	void begin();
	void end();			//Re-activates screen.
	void suspend();		//Restores active screen
	void unsuspend();	//Returns to active buffer

	void set_timer(double total_secs);

	bool   is_active();

	operator double();	//Allows animation object itself to return percent()
	double proportion();	//Proportion completion from 0 to 1.

	//static bool erase_status;	//set with set_animation_erase
	//static int  fps;			//set with set_animation_frames_per_second
	//static bool repeat_status;	//set with set_animation_repeat

	friend void set_animation_frames_per_second(int frames_per_second);

protected:
	bool active_status;
	int  total_frames;
	int  frame_on_buffer;
	double clock_begin_time;
	double suspend_begin_time;

};//End of class Animator

extern Animator gfx_animation;

//This next macro gets redefined in the screen saver package,
//so we comment it out here when we do screen savers.
#define ANIMATE(t, repeat) gfx_animation.set_timer(t); set_animation_repeat(repeat); gfx_animation.begin(); while(gfx_animation.animate())

//This nifty macro should be given a body for any one-time set-up stuff
//inside an ANIMATE for a project that will become a Screen Saver.
#define FIRST_TIME static int first_time_ = 0; if(first_time_-- == 0)

#define InCurrentAnimation (Animator::current -> proportion())
//*****************************************************************************
//****************************THE Grid CLASS***********************************
//*****************************************************************************
/*A Grid object defines a region of the screen consisting of tiled, indexed
rectangles and provides a set of functions for dealing with them.
A Grid object is defined by
 1. a bounding rectangle
 2. a number of rows and columns
 3. starting indices
*/
class GridCell;
class Grid
{
  public:

//Grid Constructors
	Grid();
	Grid(int num_rows, int num_cols);
	Grid(int x, int y, int rx, int ry, int num_rows, int num_cols);
	Grid(Point center, int rx, int ry, int num_rows, int num_cols);
	Grid(Point left_top_pt, Point right_bottom_pt, int num_rows, int num_cols);
	Grid(Grid grid, int num_rows, int num_cols);

//Obtaining GridCells
	GridCell operator()(int index);
	GridCell operator()(int row, int col);
	GridCell cursor_cell();		//returns cell nearest cursor
	GridCell random_cell();

//Grid Drawing Functions
	void draw(bool border = true, bool gridlines = true);
	void draw(int inset_percent);
	void draw(int inset_percent_x, int inset_percent_y);
	void draw_circle();
	void draw_circle(int inset_percent);
	void draw_circle(int inset_percent_x, int inset_percent_y);
	void draw_circle_filled();
	void draw_circle_filled(int inset_percent);
	void draw_circle_filled(int inset_percent_x, int inset_percent_y);
	void draw_filled(int inset_percent = 0);
	void draw_filled(int inset_percent_x, int inset_percent_y);
	void draw_ellipse();
	void draw_ellipse(int inset_percent);
	void draw_ellipse(int inset_percent_x, int inset_percent_y);
	void draw_ellipse_filled();
	void draw_ellipse_filled(int inset_percent);
	void draw_ellipse_filled(int inset_percent_x, int inset_percent_y);

	void draw_text(std::string str);
	void draw_text(char ch);
	void draw_text(int n);
	void draw_text(unsigned int n);
	void draw_text(double n);

	std::string draw_input_text(std::string prompt = "");

//Grid Mouse Input
	bool is_click(int inset_percent = 0);
	bool is_click(int inset_percent_x, int inset_percent_y);
	GridCell get_click(int inset_percent = 0);
	GridCell get_click(int inset_percent_x, int inset_percent_y);
	void remove_click(int inset_percent = 0);
	void remove_click(int inset_percent_x, int inset_percent_y);
	
//Grid Cell Reference Properties

	//accessors of cell reference

	int first_row();
	int first_col();
	int first_index();
	int last_row();
	int last_col();
	int last_index();

	//mutators of cell reference

	void set_first_row(int row);
	void set_first_col(int col);
	void set_first_index(int index);
	void set_first(int first);
	
	//utilities of cell reference

	int rowcol_to_index(int row, int col);
	int index_to_row(int index);
	int index_to_col(int index);

//Grid Dimension Properties

	//accessors of dimension

	int num_rows();
	int num_cols();
	int num_cells();

	//mutators of dimension

	void set_num_rows(int num_rows);
	void set_num_cols(int num_cols);
	void set_dimensions(int num_rows, int num_cols);

//Grid Position Properties

	//accessors of position

	Point center();
	Point midpoint();
	int x();
	int y();
	int left();
	int right();
	int top();
	int bottom();
	Point left_top();
	Point right_bottom();
	Point left_bottom();
	Point right_top();

	//mutators of position

	void set_center(int x, int y);
	void set_center(Point center);
	void set_position(int x, int y, int rx, int ry);
	void set_position(Point center, int rx, int ry);
	void set_position(Point left_top_pt, Point right_bottom_pt);
	void set_position(Grid grid);

	//utilities of position

	bool has_point(int x, int y, int inset_percent = 0);
	bool has_point(int x, int y, int inset_percent_x, int inset_percent_y);
	bool has_point(Point p, int inset_percent = 0);
	bool has_point(Point p, int inset_percent_x, int inset_percent_y);
	GridCell point_to_cell(int x, int y);	//Returns cell nearest point
	GridCell point_to_cell(Point p);

//Grid Size Functions
	int cell_width(int inset_percent = 0);
	int cell_width(int inset_percent_x, int inset_percent_y);
	int cell_height(int inset_percent = 0);
	int cell_height(int inset_percent_x, int inset_percent_y);
	int cell_radius_x(int inset_percent = 0);
	int cell_radius_x(int inset_percent_x, int inset_percent_y);
	int cell_radius_y(int inset_percent = 0);
	int cell_radius_y(int inset_percent_x, int inset_percent_y);

	int width(int inset_percent = 0);
	int width(int inset_percent_x, int inset_percent_y);
	int height(int inset_percent = 0);
	int height(int inset_percent_x, int inset_percent_y);
	int radius_x(int inset_percent = 0);
	int radius_x(int inset_percent_x, int inset_percent_y);
	int radius_y(int inset_percent = 0);
	int radius_y(int inset_percent_x, int inset_percent_y);

//Subgrids
	Grid subgrid(int first_row, int first_col, int last_row, int last_col);
	Grid subgrid(int first_index, int last_index);
	Grid subgrid(GridCell left_top_cell, GridCell right_bottom_cell);
	Grid grid_row(int which_row);
	Grid grid_col(int which_col);

//General Grid Functions
	void erase();	//erases interior inset by 1 pixel from boundary
	void erase(Color color);
	void erase(int inset_percent);
	void erase(int inset_percent_x, int inset_percent_y);
	Point random_point(int inset_percent = 0);
	Point random_point(int inset_percent_x, int inset_percent_y);

//Grid Cursor Rescale Functions
	int cursor_x_rescaled(int inset_percent = 0);
	int cursor_x_rescaled(int inset_percent_x, int inset_percent_y);
	int cursor_y_rescaled(int inset_percent = 0);
	int cursor_y_rescaled(int inset_percent_x, int inset_percent_y);
	Point cursor_point_rescaled(int inset_percent = 0);
	Point cursor_point_rescaled(int inset_percent_x, int inset_percent_y);

//Grid Internal Helper Functions
  protected:
	Point inset_left_top(int percent_x, int percent_y);
	Point inset_right_bottom(int percent_x, int percent_y);

//Grid Data members
  protected:
	int m_first_row, m_first_col, m_first_index;	//Cell reference
	int m_num_rows, m_num_cols;						//Dimension
	Point m_left_top, m_right_bottom;				//Position
};
//*****************************************************************************
//*************************THE GridCell CLASS**********************************
//*****************************************************************************
class GridCell: public Grid
{
//GridCell Constructors
  public:
	GridCell();		  //The default constructor is public
  friend class Grid;  //Only Grids should be able to construct actual GridCells.
  private:
	GridCell(Point left_top, Point right_bottom, int row, int col, int index);

  public:

//GridCell Data Members
  public:
	int row, col, index;
};
//*****************************************************************************
//*************************THE SquareGrid CLASS********************************
//*****************************************************************************
//SquareGrid GridCells will be square, but the entire SquareGrid may not be.
class SquareGrid: public Grid
{
  public:

//SquareGrid Constructors
	SquareGrid();
	SquareGrid(int num_rows, int num_cols);
	SquareGrid(int x, int y, int rx, int ry, int num_rows, int num_cols);
	SquareGrid(Point center, int rx, int ry, int num_rows, int num_cols);
	SquareGrid(Point left_top, Point right_bottom, int num_rows, int num_cols);
	SquareGrid(Grid grid, int num_rows, int num_cols);
	SquareGrid(GridCell cell);
	SquareGrid(int x, int y, int rx, int num_rows, int num_cols);
	SquareGrid(Point center, int rx, int num_rows, int num_cols);

//SquareGrid Dimension mutators

	void set_num_rows(int num_rows);
	void set_num_cols(int num_cols);
	void set_dimensions(int num_rows, int num_cols);

//SquareGrid Position Mutators
	void set_position(int x, int y, int rx, int ry);
	void set_position(Point center, int rx, int ry);
	void set_position(Point left_top, Point right_bottom);
	void set_position(Grid grid);
	void set_position(int x, int y, int rx);
	void set_position(Point center, int rx);

//SquareGrid Size Function
	int cell_radius(int inset_percent = 0);

//SquareGrid Internal Helper Function
	int calculate_ry(int rx);
};

//*****************************************************************************
//*************************THE Ring/Cell CLASSES*******************************
//*****************************************************************************

/*A Ring object defines a region of the screen consisting of indexed
circles arranged in a large circle and provides a set of functions for
dealing with them.  A Ring object is defined by
 1. a center point and radius for the circular arrangement
 2. a number of cells
 3. a starting index
 4. a starting angle
 */

class RingCell;	//Forward declaration
class Ring
{
  public:

//Ring Constructors
	Ring();
	Ring(int num_cells);
	Ring(int x, int y, int r, int num_cells, int ang = 0);
	Ring(Point center, int r, int num_cells, int ang = 0);

//Obtaining RingCells
	RingCell operator()(int index);
	RingCell cursor_cell();		//returns cell nearest cursor
	RingCell random_cell();

//Ring Drawing Functions
	void draw(int inset_percent = 0);
	void draw_filled(int inset_percent = 0);
	void draw_circle(int inset_percent = 0);
	void draw_circle_filled(int inset_percent = 0);

	void draw_text(std::string str);
	void draw_text(char ch);
	void draw_text(int n);
	void draw_text(unsigned int n);
	void draw_text(double n);

	std::string draw_input_text(std::string prompt = "");

//Ring Mouse Input
	bool is_click(int inset_percent = 0);
	RingCell get_click(int inset_percent = 0);
	void remove_click(int inset_percent = 0);

//Ring Cell Reference Properties

	//accessors of cell reference

	int first_index();
	int last_index();

	//mutators of cell reference

	void set_first_index(int index);
	void set_first(int index);

//Ring Dimension Properties

	//accessor of dimension

	int num_cells();
	
	//mutators of dimension

	void set_num_cells(int num_cells);

//Ring Position Properties

	//accessors of position

	Point center();
	Point midpoint();
	operator Point();
	int x();
	int y();
	int angle();

	//mutators of position

	void set_center(int x, int y);
	void set_center(Point center);
	void set_position(int x, int y, int radius);
	void set_position(int x, int y, int r, int ang);
	void set_position(Point center, int radius);
	void set_position(Point center, int radius, int angle);
	void set_angle(int angle);

	//utilities of position

	bool has_point(int x, int y, int inset_percent = 0);
	bool has_point(Point p, int inset_percent = 0);
	RingCell point_to_cell(int x, int y);
	RingCell point_to_cell(Point p);
	int index_to_angle(int index);

//Ring Size Functions
	int cell_radius(int inset_percent = 0);
	int radius(int inset_percent = 0);

//General Ring Functions
	void erase();	//erases interior inset by 1 pixel from boundary
	void erase(Color color);
	void erase(int inset_percent);
	Point random_point(int inset_percent = 0);

//For compatibility with Framework placement classes
	int index_to_row(int index);
	int index_to_col(int index);
	int rowcol_to_index(int row, int col);

//Ring Data members
  protected:
	int m_num_cells;		//Dimension
	int m_first_index;		//Cell Reference
	Point m_center;			//Position
	int m_radius;
	int m_angle;

};
//*****************************************************************************
//*************************THE RingCell CLASS**********************************
//*****************************************************************************
class RingCell: public Ring
{
	friend class Ring;	//Only Ring should be able to construct cells.
  private:

//RingCell Constructors
	RingCell();
	RingCell(Point center, int radius, int index);

//RingCell Data Members
  public:
	int index;
};

//*****************************************************************************
//****************************THE Hex CLASS************************************
//*****************************************************************************
//A Hex object defines a region of the screen consisting of tiled, indexed
//hexagons and provides a set of functions for dealing with them.
//A Hex object is defined by
// 1. a center and horizontal radius
// 2. a number of rows and columns
// 3. starting indices

class HexCell;
class Hex
{
  public:

//Hex Constructors
	Hex();
	Hex(int num_rows, int num_cols);
	Hex(int x, int y, int rx, int ry, int num_rows, int num_cols);
	Hex(Point center, int rx, int ry, int num_rows, int num_cols);
	Hex(Point left_top, Point right_bottom, int num_rows, int num_cols);
	Hex(int x, int y, int rx, int num_rows, int num_cols);
	Hex(Point center, int rx, int num_rows, int num_cols);

//The Underlying Grid
	Grid grid();

//Obtaining HexCells
	HexCell operator()(int row, int col);
	HexCell operator()(int index);
	HexCell cursor_cell();
	HexCell random_cell();

//Hex Drawing Functions
	void draw(bool draw_grid = false, bool hexlines = true);
	void draw(int inset_percent);
	void draw_circle(int inset_percent = 0);
	void draw_circle_filled(int inset_percent = 0);
	void draw_filled(int inset_percent = 0);

	void draw_text(std::string str);
	void draw_text(char ch);
	void draw_text(int n);
	void draw_text(unsigned int n);
	void draw_text(double n);

	std::string draw_input_text(std::string prompt = "");

//Hex Mouse Input
	bool is_click(int inset_percent = 0);
	HexCell get_click(int inset_percent = 0);
	void remove_click(int inset_percent = 0);

//Hex Cell Reference Properties

	//accessors of cell reference

	int first_row();
	int first_row(int which_col);
	int first_col();
	int first_col(int which_row);
	int first_index();
	int last_row();
	int last_row(int which_col);
	int last_col();
	int last_col(int which_row);
	int last_index();

	//mutators of cell reference

	void set_first_row(int row);
	void set_first_col(int col);
	void set_first_index(int index);
	void set_first(int first);

	//utilities of cell reference

	int rowcol_to_index(int row, int col);
	int index_to_row(int index);
	int index_to_col(int index);

//Hex Dimension Properties

	//accessors of dimension

	int num_rows();
	int num_cols();
	int num_cells();

	//mutators of dimension

	void set_num_rows(int num_rows);
	void set_num_cols(int num_cols);
	void set_dimensions(int num_rows, int num_cols);

//Hex Position Properties

	//accessors of position

	Point center();
	Point midpoint();
	int x();
	int y();

	//mutators of position

	void set_center(int x, int y);
	void set_center(Point center);
	void set_position(int x, int y, int rx, int ry);
	void set_position(Point center, int rx, int ry);
	void set_position(Point left_top, Point right_bottom);
	void set_position(int x, int y, int rx);
	void set_position(Point center, int rx);
	
	//utilities of position

	bool has_point(int x, int y, int inset_percent = 0);
	bool has_point(Point p, int inset_percent = 0);
	HexCell point_to_cell(int x, int y);
	HexCell point_to_cell(Point p);

//Hex Size Functions
	int cell_in_radius (int inset_percent = 0);
	int cell_out_radius(int inset_percent = 0);
	
	int grid_radius_x(int inset_percent = 0);
	int grid_radius_y(int inset_percent = 0);

//Subhex
	Hex subhex(int first_row, int first_col, int last_row, int last_col);
	Hex subhex(int first_index, int last_index);
	Hex subhex(HexCell left_top_cell, HexCell right_bottom_cell);
	Hex hex_row(int which_row);
	Hex hex_col(int which_col);

//General Hex Functions
	void erase();	//erases interior inset by 1 pixel from boundary
	void erase(Color color);
	void erase(int inset_percent);
	Point random_point(int inset_percent = 0);

//Hex Internal Helper Function
	int calculate_ry(int rx);

//Hex Data members

  protected:
	int m_first_row, m_first_col, m_first_index;	//Cell reference
	int m_num_rows, m_num_cols;						//Dimension
	Point m_center;									//Position
	int m_rx;
};
//*****************************************************************************
//*************************THE HexCell CLASS***********************************
//*****************************************************************************
class HexCell: public Hex
{
//HexCell Constructors
  public:
	HexCell();		  //The default constructor is public

  friend class Hex;  //Only Hex should be able to construct actual HexCells.
  private:
	HexCell(Point center, int rx, int row_, int col_, int index_);

  public:

//HexCell Size Functions
	int in_radius (int inset_percent = 0);
	int out_radius(int inset_percent = 0);

//HexCell Data Members
  public:
	int row, col, index;
};


//*****************************************************************************
//*************************THE Shape CLASSES***********************************
//*****************************************************************************
//A Shape is essentially an elliptic_ngon.  With parameter out == -100, it fits 
//in the bounding ellipse, with out == 100, it is circumscribed, and with out between
//these extremes, the Shape varies in size accordingly.  With out == 0, or with n == 0, 
//we imagine that we have an ellipse, but in practice we treat it as a polygon 
//with a large number of sides.  This gives the ability to draw rotated "ellipses".
//Shapes also provide the ability to morph between different Shapes via range.

//If the users ever includes both windows.h and shape.h in the same file, 
//there will be conflicting definitions for Rectangle and the windows GDI 
//Rectangle will "win".  In this case, to use the shape.h's Rectangle, 
//prefix Rectangle with the word class as in "class Rectangle" whenever the 
//Rectangle Shape is needed.

class Shape
{
public: 
	Shape(int x_ = 0, int y_ = 0, int rx_ = 0, int ry_ = 0, int n_ = 4, int rot_ = 0, int out_ = -100);
	Shape(Point center_ , int rx_, int ry_, int n_, int rot_ = 0, int out_ = -100);
	Shape(Point left_top, Point right_bottom, int n_, int rot_ = 0, int out_ = -100);
	
	void draw();
	void draw_filled();
	void draw_3D();
	void draw_3D_filled();
	void draw_P3D();
	void draw_P3D_filled();

	Point point(int angle);
	Point point(Point ray_point);
	void vertices(Point* vertex_array);

	static int max_sides;

public:
	int x, y, rx, ry, num_sides, rot, out;
};


class Circle: public Shape
{
public:
	Circle(int x_, int y_, int r, int rot_ = 0);
	Circle(Point center_ , int r, int rot_ = 0);
};
class Ellipse: public Shape
{
public:
	Ellipse(int x_, int y_, int rx_, int ry_, int rot_ = 0);
	Ellipse(Point center_ , int rx_, int ry_, int rot_ = 0);
	Ellipse(Point left_top, Point right_bottom, int rot_ = 0);
};
class EllipticNgon: public Shape
{
public:
	EllipticNgon(int x_, int y_, int rx_, int ry_, int n_, int rot_ = 0);
	EllipticNgon(Point center_ , int rx_, int ry_, int n_, int rot_ = 0);
	EllipticNgon(Point left_top, Point right_bottom, int n_, int rot_ = 0);
};
class EllipticOutgon: public Shape
{
public:
	EllipticOutgon(int x_, int y_, int rx_, int ry_, int n_, int rot_ = 0);
	EllipticOutgon(Point center_ , int rx_, int ry_, int n_, int rot_ = 0);
	EllipticOutgon(Point left_top, Point right_bottom, int n_, int rot_ = 0);
};
class Ngon: public Shape
{
public:
	Ngon(int x_, int y_, int r, int n_, int rot_ = 0);
	Ngon(Point center_ , int r, int n_, int rot_ = 0);
};
class Outgon: public Shape
{
public:
	Outgon(int x_, int y_, int r, int n_, int rot_ = 0);
	Outgon(Point center_ , int r, int n_, int rot_ = 0);
};
class Rectangle: public Shape
{
public:
	Rectangle(int x_, int y_, int rx_, int ry_, int rot_ = 0);
	Rectangle(Point center_ , int rx_, int ry_, int rot_ = 0);
	Rectangle(Point left_top, Point right_bottom, int rot_ = 0);
};
class Square: public Shape
{
public:
	Square(int x_, int y_, int r, int rot_ = 0);
	Square(Point center_ , int r, int rot_ = 0);
};

class VariableShape
{
public:
	//Constructor, allows specification of a range of legal values
	VariableShape(double seconds = 5.0, Shape lim1 = Shape(PerX(25),PerY(25),PerX(5),PerY(5),0,0,-100), Shape lim2 = Shape(PerX(75),PerY(75),PerX(35),PerY(35),8,360,100), bool is_fixed_rate = false);
	VariableShape(double seconds, bool is_fixed_rate);

	//Conversion operator allows VariableShapes to be used as Shape.
	operator Shape();
	
	void begin();		//Rebegin current transition
	void end();			//End current transition
	void reset();		//Pick new random transition
	void reset(Shape start);	//Pick new end but begin from start
	void reverse();		//Change direction of transition

	//Change the limits of possible values
	//Note that we will still finish the present transition
	void set_limits(Shape lim1, Shape lim2, bool reset_it = true); 

	//How long between changes?
	double get_period();
	void set_period(double seconds);

	double get_rate();
	void multiply_rate_by(double multiplier);
	void set_rate(double seconds);
	void set_rate(double dist, double seconds);

	double distance();
	int percent();
	double proportion();

public: //Data members
	Shape limit1, limit2;
	Shape old_val, new_val;

private:
	bool fixed_rate_mode;
	double period;
	double rate;
	int last_change_time;
	void calc_period();
};

//*****************************************************************************
//*************************THE RoundedShape CLASS******************************
//*****************************************************************************
/*
The RoundedShape class has a draw function which interprets the out parameter 
as a degree of roundedness.  Think of out == -100 as being an ngon.  As out 
increases to 0, the ngon is smashed against the bounding ellipse.  As out 
increases toward 100, the RoundedShape becomes increasingly an outgon.

The RoundedShape class gives a different interpretation to the parameter space 
of a Shape in terms of the topology of that space for the range function.  Think 
of an (num_sides, out) plane in which num_sides == 0 or out == 0 describe the 
elliptical object.  As we move between out > 0 to out < 0, or between differing 
values of n (and hence through n == 0), we must pass through the elliptic object.

One advantage of this system is that we can morph between a hexagon and 
a rectangle without jumping discontinously through a pentagon.

There is still a lot that can be implemented, such as the 3D draws, 
and range variations.  The draw_filled has some "artifacts" in it.

Presently, RoundedShapes cannot deal with rotations.

  Sample code in a animation loop:
	  	range(
		   RoundedShape(MidPt, 100, 200, 4, 75),  //defined directly
		   RoundedShape(Ngon(PerPt(75), 150, 6))  //cast from a Shape
		).draw();
  */
class RoundedShape: public Shape
{
public:
	//The rotation member of Shape is unsettable and ignored.
	RoundedShape(int x_ = 0, int y_ = 0, int rx_ = 0, int ry_ = 0, int n_ = 4, int out_ = -100);
	RoundedShape(Point center_ , int rx_, int ry_, int n_, int out_ = -100);
	RoundedShape(Point left_top, Point right_bottom, int n_, int out_ = -100);
	RoundedShape(Shape s);
	
	void draw();
	void draw_filled();
	void draw_3D();
	void draw_3D_filled();
	void draw_P3D();
	void draw_P3D_filled();

private:
	void get_parameters(int& new_rx, int& new_ry, int& new_out, 
						Point* verts, Point* behinds, Point* aheads);
};

class VariableRoundedShape
{
public:
	//Constructor, allows specification of a range of legal values
	VariableRoundedShape(double seconds = 5.0, RoundedShape lim1 = RoundedShape(PerX(25),PerY(25),PerX(5),PerY(5),0,-100), RoundedShape lim2 = Shape(PerX(75),PerY(75),PerX(35),PerY(35),8,100), bool is_fixed_rate = false);
	VariableRoundedShape(double seconds, bool is_fixed_rate);

	//Conversion operator allows VariableRoundedShapes to be used as RoundedShape.
	operator RoundedShape();
	
	void begin();		//Rebegin current transition
	void end();			//End current transition
	void reset();		//Pick new random transition
	void reset(RoundedShape start);	//Pick new end but begin from start
	void reverse();		//Change direction of transition

	//Change the limits of possible values
	//Note that we will still finish the present transition
	void set_limits(RoundedShape lim1, RoundedShape lim2, bool reset_it = true); 

	//How long between changes?
	double get_period();
	void set_period(double seconds);

	double get_rate();
	void multiply_rate_by(double multiplier);
	void set_rate(double seconds);
	void set_rate(double dist, double seconds);

	double distance();
	int percent();
	double proportion();

public: //Data members
	RoundedShape limit1, limit2;
	RoundedShape old_val, new_val;

private:
	bool fixed_rate_mode;
	double period;
	double rate;
	int last_change_time;
	void calc_period();
};
//*****************************************************************************
//*************************THE VariableXXX CLASSES*****************************
//*****************************************************************************
//It is important to keep a separate default constructor.  This allows us to 
//declare VariableXXX arrays at the global level and not worry if any of their
//parameter default values have been initialized yet.  The array elements can 
//be given values in code as in my_variable_point[i] = VariablePoint(...)
class VariableColor
{
public:
	//Constructor, allows specification of a range of legal values
	VariableColor();
	VariableColor(double seconds, Color lim1 = BLACK, Color lim2 = WHITE, bool is_fixed_rate = false);
	VariableColor(double seconds, bool is_fixed_rate);

	//Conversion operator allows VariableColors to be used as Color.
	operator Color();
	
	void begin();		//Rebegin current transition
	void end();			//End current transition
	void reset();		//Pick new random transition
	void reset(Color start);	//Pick new end but begin from start
	void reverse();		//Change direction of transition

	//Change the limits of possible values
	//Note that we will still finish the present transition
	void set_limits(Color lim1, Color lim2, bool reset_it = true); 

	//How long between changes?
	double get_period();
	void set_period(double seconds);

	double get_rate();
	void multiply_rate_by(double multiplier);
	void set_rate(double seconds);
	void set_rate(double dist, double seconds);

	double distance();
	int percent();
	double proportion();

public: //Data members
	Color limit1, limit2;
	Color old_val, new_val;

private:
	bool fixed_rate_mode;
	double period;
	double rate;
	int last_change_time;
	void calc_period();
};
////////////////////////////////
//The key is to remember to use random_continuous and range_continuous
class VariableDouble
{
public:
	//Constructor, allows specification of a range of legal values
	VariableDouble();
	VariableDouble(double seconds, double low = 0.0, double high = 1.0, bool is_fixed_rate = false);
	VariableDouble(double seconds, bool is_fixed_rate);

	//Conversion operator allows VariableDoubles to be used as int.
	operator double();
	
	void begin();		//Rebegin current transition
	void end();			//End current transition
	void reset();		//Pick new random transition
	void reset(double start);	//Pick new end but begin from start
	void reverse();		//Change direction of transition

	//Change the limits of possible values
	//Note that we will still finish the present transition
	void set_limits(double low, double high, bool reset_it = true);

	//How long between changes?
	double get_period();
	void set_period(double seconds);

	double get_rate();
	void multiply_rate_by(double multiplier);
	void set_rate(double seconds);
	void set_rate(double dist, double seconds);

	double difference();
	double distance();
	int percent();
	double proportion();

	//These let us access facts about the limits rectangle
	double low();
	double high();

public: //Data members
	double limit1, limit2;
	double old_val, new_val;

private:
	bool fixed_rate_mode;
	double period;
	double rate;
	int last_change_time;
	void calc_period();
};
////////////////////////////////
class VariableInt
{
public:
	//Constructor, allows specification of a range of legal values
	VariableInt();
	VariableInt(double seconds, int low = 0, int high = 100, bool is_fixed_rate = false);
	VariableInt(double seconds, bool is_fixed_rate);

	//Conversion operator allows VariableInts to be used as int.
	operator int();
	
	void begin();		//Rebegin current transition
	void end();			//End current transition
	void reset();		//Pick new random transition
	void reset(int start);	//Pick new end but begin from start
	void reverse();		//Change direction of transition

	//Change the limits of possible values
	//Note that we will still finish the present transition
	void set_limits(int low, int high, bool reset_it = true);

	//How long between changes?
	double get_period();
	void set_period(double seconds);

	double get_rate();
	void multiply_rate_by(double multiplier);
	void set_rate(double seconds);
	void set_rate(double dist, double seconds);

	int difference();
	double distance();
	int percent();
	double proportion();

	//These let us access facts about the limits rectangle
	int low();
	int high();

public: //Data members
	int limit1, limit2;
	int old_val, new_val;

private:
	bool fixed_rate_mode;
	double period;
	double rate;
	int last_change_time;
	void calc_period();
};


////////////////////////////////
class VariablePoint
{
public:
	//Constructor, allows specification of a range of legal values
	VariablePoint();
	VariablePoint(double seconds, Point left_top = MinPt, Point right_bottom = MaxPt, bool is_fixed_rate = false);
	VariablePoint(double seconds, bool is_fixed_rate);

	//Conversion operator allows VariablePoints to be used as Point.
	operator Point();

	//These give us the x and y coordinates of the current Point.
	int x();
	int y();
	
	void begin();		//Rebegin current transition
	void end();			//End current transition
	void reset();		//Pick new random transition
	void reset(Point start);	//Pick new end but begin from start
	void reverse();		//Change direction of transition
	void show_path();	//Draw a line between endpoints of current transition

	//Change the limits of possible values
	//Note that we will still finish the present path
	void set_limits(int x, int y, int rx, int ry, bool reset_it = true);
	void set_limits(Point center, int rx, int ry, bool reset_it = true);
	void set_limits(Point left_top, Point right_bottom, bool reset_it = true);
	void set_limits(int inset, bool reset_it = true);
	void set_limits(int inset_x, int inset_y, bool reset_it = true);

	//How long between changes?
	double get_period();
	void set_period(double seconds);

	double get_rate();
	void multiply_rate_by(double multiplier);
	void set_rate(double seconds);
	void set_rate(double dist, double seconds);

	Point difference();
	double distance();
	int percent();
	double proportion();

	//These let us access facts about the limits rectangle
	Point center();
	int rx();
	int ry();
	int left();
	int right();
	int top();
	int bottom();
	Point left_top();
	Point right_bottom();

public: //Data members
	Point limit1, limit2;
	Point old_val, new_val;

private:
	bool fixed_rate_mode;
	double period;
	double rate;
	int last_change_time;
	void calc_period();
};
//*****************************************************************************
//*************************FREE FUNCTIONS**********************************
//*****************************************************************************
bool animation_between(int start_percent, int stop_percent);
void animation_display();
void animation_end();
int animation_percent();
double animation_proportion();
double circle_angle(int x, int y, int rayPt_x, int rayPt_y);
double circle_angle(Point center, int rayPt_x, int rayPt_y);
double circle_angle(int x, int y, Point rayPt);
double circle_angle(Point center, Point rayPt);
Point circle_point(int x, int y, int r, int angle);
Point circle_point(Point center, int r, int angle);
Point circle_point(int x, int y, int r, Point ray_point);
Point circle_point(Point center, int r, Point ray_point);
void  close_window();

template <typename S, typename T>
void copy_array(S* source, T* t, int n)
{
	for(int i = 0; i < n; i++)
		t[i] = source[i];
}

void copy_block(Point left_top_src, Point right_bottom_src, Point left_top_dest, int src_page = -1, int dest_page = -1);
void copy_block_stretch(Point left_top_src, Point right_bottom_src, Point left_top_dest, Point right_bottom_dest, int src_page = -1, int dest_page = -1);
void copy_buffer_to_screen();
void copy_buffer2_to_buffer();
void copy_buffer2_to_screen();
void copy_page(int src_page, int dest_page);
Point cursor_point();
Point cursor_point_rescaled(int x, int y, int rx, int ry);
Point cursor_point_rescaled(Point center, int rx, int ry);
Point cursor_point_rescaled(Point left_top, Point right_bottom);
void cursor_hide();
void cursor_show();
int cursor_x();
int cursor_x_rescaled(int low, int high);
double cursor_x_rescaled_continuous(int low, int high);
int cursor_y();
int cursor_y_rescaled(int low, int high);
double cursor_y_rescaled_continuous(int low, int high);
void   delay(int millisecs);
double distance(int x1, int y1, int x2, int y2);
double distance(Point p, Point q);
double distance(int x1, int y1, Point q);
double distance(Point p, int x2, int y2);
double distance(Color c1, Color c2);
double distance(Shape s, Shape t);

//*****************************************************************************
//**************************Drawing Functions**********************************
//*****************************************************************************
//Drawing circular arcs
void draw_arc(int x, int y, int r, int start_angle, int stop_angle);
void draw_arc(Point center, int r, int start_angle, int stop_angle);
void draw_arc(int x, int y, int r, Point start_point, Point stop_point);
void draw_arc(Point center, int r, Point start_point, Point stop_point);

void draw_arc_filled(int x, int y, int r, int start_angle, int stop_angle);
void draw_arc_filled(Point center, int r, int start_angle, int stop_angle);
void draw_arc_filled(int x, int y, int r, Point start_point, Point stop_point);
void draw_arc_filled(Point center, int r, Point start_point, Point stop_point);

void draw_arc_3D(int x, int y, int r, int start_angle, int stop_angle);
void draw_arc_3D(Point center, int r, int start_angle, int stop_angle);
void draw_arc_3D(int x, int y, int r, Point start_point, Point stop_point);
void draw_arc_3D(Point center, int r, Point start_point, Point stop_point);

void draw_arc_3D_filled(int x, int y, int r, int start_angle, int stop_angle);
void draw_arc_3D_filled(Point center, int r, int start_angle, int stop_angle);
void draw_arc_3D_filled(int x, int y, int r, Point start_point, Point stop_point);
void draw_arc_3D_filled(Point center, int r, Point start_point, Point stop_point);

void draw_arc_P3D(int x, int y, int r, int start_angle, int stop_angle);
void draw_arc_P3D(Point center, int r, int start_angle, int stop_angle);
void draw_arc_P3D(int x, int y, int r, Point start_point, Point stop_point);
void draw_arc_P3D(Point center, int r, Point start_point, Point stop_point);

void draw_arc_P3D_filled(int x, int y, int r, int start_angle, int stop_angle);
void draw_arc_P3D_filled(Point center, int r, int start_angle, int stop_angle);

//Drawing bezier curves
void draw_bezier(Point begin, Point c1, Point c2, Point end);	//Single curve

void draw_bezier(Point* all_points, int array_size);			//"Poly-bezier"
void draw_bezier(VariablePoint* all_points, int array_size);

void draw_bezier(Point* pts, int num_points, Point* c1, Point* c2);	//"Poly-bezier" broken down
void draw_bezier(VariablePoint* pts, int num_points, VariablePoint* c1, VariablePoint* c2);	

	//This next determines most initial control points of a poly-bezier from the previous 
	//second control point so that the pair and the intermediate endpoint are collinear.
	//This results in smooth curves at the joins.
void draw_bezier(Point* pts, int num_points, Point c1, Point* c2, int percent = 100);
void draw_bezier(VariablePoint* pts, int num_points, Point c1,
				 VariablePoint* c2, int percent = 100);

//Drawing circles
void draw_circle(int x, int y, int r);
void draw_circle(Point center, int r);
void draw_circle_filled(int x, int y, int r);
void draw_circle_filled(Point center, int r);

void draw_circle_3D(int x, int y, int r);
void draw_circle_3D(Point center, int r);
void draw_circle_3D_filled(int x, int y, int r);
void draw_circle_3D_filled(Point center, int r);

void draw_circle_P3D(int x, int y, int r);
void draw_circle_P3D(Point center, int r);
void draw_circle_P3D_filled(int x, int y, int r);
void draw_circle_P3D_filled(Point center, int r);

//Drawing ellipses
void draw_ellipse(Point left_top, Point right_bottom, int rot = 0);
void draw_ellipse(int x, int y, int rx, int ry, int rot = 0);
void draw_ellipse(Point center, int rx, int ry, int rot = 0);
void draw_ellipse_filled(Point left_top, Point right_bottom, int rot = 0);
void draw_ellipse_filled(int x, int y, int rx, int ry, int rot = 0);
void draw_ellipse_filled(Point center, int rx, int ry, int rot = 0);

void draw_ellipse_3D(Point left_top, Point right_bottom, int rot = 0);
void draw_ellipse_3D(int x, int y, int rx, int ry, int rot = 0);
void draw_ellipse_3D(Point center, int rx, int ry, int rot = 0);
void draw_ellipse_3D_filled(Point left_top, Point right_bottom, int rot = 0);
void draw_ellipse_3D_filled(int x, int y, int rx, int ry, int rot = 0);
void draw_ellipse_3D_filled(Point center, int rx, int ry, int rot = 0);

void draw_ellipse_P3D(Point left_top, Point right_bottom, int rot = 0);
void draw_ellipse_P3D(int x, int y, int rx, int ry, int rot = 0);
void draw_ellipse_P3D(Point center, int rx, int ry, int rot = 0);
void draw_ellipse_P3D_filled(Point left_top, Point right_bottom, int rot = 0);
void draw_ellipse_P3D_filled(int x, int y, int rx, int ry, int rot = 0);
void draw_ellipse_P3D_filled(Point center, int rx, int ry, int rot = 0);

//Drawing elliptic arcs
void draw_elliptic_arc(Point left_top, Point right_bottom, Point start_point, Point stop_point);
void draw_elliptic_arc(int x, int y, int rx, int ry, Point start_point, Point stop_point);
void draw_elliptic_arc(Point center, int rx, int ry, Point start_point, Point stop_point);

void draw_elliptic_arc(Point left_top, Point right_bottom, int start_angle, int end_angle);
void draw_elliptic_arc(int x, int y, int rx, int ry, int start_angle, int end_angle);
void draw_elliptic_arc(Point center, int rx, int ry, int start_angle, int end_angle);

void draw_elliptic_arc_filled(Point left_top, Point right_bottom, Point start_point, Point stop_point);
void draw_elliptic_arc_filled(int x, int y, int rx, int ry, Point start_point, Point stop_point);
void draw_elliptic_arc_filled(Point center, int rx, int ry, Point start_point, Point stop_point);

void draw_elliptic_arc_filled(Point left_top, Point right_bottom, int start_angle, int end_angle);
void draw_elliptic_arc_filled(int x, int y, int rx, int ry, int start_angle, int end_angle);
void draw_elliptic_arc_filled(Point center, int rx, int ry, int start_angle, int end_angle);

void draw_elliptic_arc_3D(Point left_top, Point right_bottom, Point start_point, Point stop_point);
void draw_elliptic_arc_3D(int x, int y, int rx, int ry, Point start_point, Point stop_point);
void draw_elliptic_arc_3D(Point center, int rx, int ry, Point start_point, Point stop_point);

void draw_elliptic_arc_3D(Point left_top, Point right_bottom, int start_angle, int end_angle);
void draw_elliptic_arc_3D(int x, int y, int rx, int ry, int start_angle, int end_angle);
void draw_elliptic_arc_3D(Point center, int rx, int ry, int start_angle, int end_angle);

void draw_elliptic_arc_3D_filled(Point left_top, Point right_bottom, Point start_point, Point stop_point);
void draw_elliptic_arc_3D_filled(int x, int y, int rx, int ry, Point start_point, Point stop_point);
void draw_elliptic_arc_3D_filled(Point center, int rx, int ry, Point start_point, Point stop_point);

void draw_elliptic_arc_3D_filled(Point left_top, Point right_bottom, int start_angle, int end_angle);
void draw_elliptic_arc_3D_filled(int x, int y, int rx, int ry, int start_angle, int end_angle);
void draw_elliptic_arc_3D_filled(Point center, int rx, int ry, int start_angle, int end_angle);

void draw_elliptic_arc_P3D(Point left_top, Point right_bottom, Point start_point, Point stop_point);
void draw_elliptic_arc_P3D(int x, int y, int rx, int ry, Point start_point, Point stop_point);
void draw_elliptic_arc_P3D(Point center, int rx, int ry, Point start_point, Point stop_point);

void draw_elliptic_arc_P3D(Point left_top, Point right_bottom, int start_angle, int end_angle);
void draw_elliptic_arc_P3D(int x, int y, int rx, int ry, int start_angle, int end_angle);
void draw_elliptic_arc_P3D(Point center, int rx, int ry, int start_angle, int end_angle);

void draw_elliptic_arc_P3D_filled(Point left_top, Point right_bottom, Point start_point, Point stop_point);
void draw_elliptic_arc_P3D_filled(int x, int y, int rx, int ry, Point start_point, Point stop_point);
void draw_elliptic_arc_P3D_filled(Point center, int rx, int ry, Point start_point, Point stop_point);

void draw_elliptic_arc_P3D_filled(Point left_top, Point right_bottom, int start_angle, int end_angle);
void draw_elliptic_arc_P3D_filled(int x, int y, int rx, int ry, int start_angle, int end_angle);
void draw_elliptic_arc_P3D_filled(Point center, int rx, int ry, int start_angle, int end_angle);

//Drawing elliptic_ngons
//An elliptic_ngon is a regular polygon inscribed in an ellipse.
//The angle rot in degrees rotates the ellipse.
//See also elliptic_outgons for polygons circumscribed about an ellipse.
void draw_elliptic_ngon(int x, int y, int rx, int ry, int n, int rot = 0);
void draw_elliptic_ngon(Point center, int rx, int ry, int n, int rot = 0);
void draw_elliptic_ngon(Point left_top, Point right_bottom, int n, int rot = 0);
void draw_elliptic_ngon_filled(int x, int y, int rx, int ry, int n, int rot = 0);
void draw_elliptic_ngon_filled(Point center, int rx, int ry, int n, int rot = 0);
void draw_elliptic_ngon_filled(Point left_top, Point right_bottom, int n, int rot = 0);

void draw_elliptic_ngon_3D(int x, int y, int rx, int ry, int n, int rot = 0);
void draw_elliptic_ngon_3D(Point center, int rx, int ry, int n, int rot = 0);
void draw_elliptic_ngon_3D(Point left_top, Point right_bottom, int n, int rot = 0);
void draw_elliptic_ngon_3D_filled(int x, int y, int rx, int ry, int n, int rot = 0);
void draw_elliptic_ngon_3D_filled(Point center, int rx, int ry, int n, int rot = 0);
void draw_elliptic_ngon_3D_filled(Point left_top, Point right_bottom, int n, int rot = 0);

void draw_elliptic_ngon_P3D(int x, int y, int rx, int ry, int n, int rot = 0); 
void draw_elliptic_ngon_P3D(Point center, int rx, int ry, int n, int rot = 0);
void draw_elliptic_ngon_P3D(Point left_top, Point right_bottom, int n, int rot = 0); 
void draw_elliptic_ngon_P3D_filled(int x, int y, int rx, int ry, int n, int rot = 0);
void draw_elliptic_ngon_P3D_filled(Point center, int rx, int ry, int n, int rot = 0);
void draw_elliptic_ngon_P3D_filled(Point left_top, Point right_bottom, int n, int rot = 0);

//Drawing elliptic_outgons
//An elliptic_outgon is a regular polygon circumscribed about an ellipse.
//The angle rot in degrees rotates the ellipse.
//See also elliptic_ngons for polygons inscribed in an ellipse.
void draw_elliptic_outgon(int x, int y, int rx, int ry, int n, int rot = 0);
void draw_elliptic_outgon(Point center, int rx, int ry, int n, int rot = 0);
void draw_elliptic_outgon(Point left_top, Point right_bottom, int n, int rot = 0);
void draw_elliptic_outgon_filled(int x, int y, int rx, int ry, int n, int rot = 0);
void draw_elliptic_outgon_filled(Point center, int rx, int ry, int n, int rot = 0);
void draw_elliptic_outgon_filled(Point left_top, Point right_bottom, int n, int rot = 0);

void draw_elliptic_outgon_3D(int x, int y, int rx, int ry, int n, int rot = 0);
void draw_elliptic_outgon_3D(Point center, int rx, int ry, int n, int rot = 0);
void draw_elliptic_outgon_3D(Point left_top, Point right_bottom, int n, int rot = 0);
void draw_elliptic_outgon_3D_filled(int x, int y, int rx, int ry, int n, int rot = 0);
void draw_elliptic_outgon_3D_filled(Point center, int rx, int ry, int n, int rot = 0);
void draw_elliptic_outgon_3D_filled(Point left_top, Point right_bottom, int n, int rot = 0);

void draw_elliptic_outgon_P3D(int x, int y, int rx, int ry, int n, int rot = 0); 
void draw_elliptic_outgon_P3D(Point center, int rx, int ry, int n, int rot = 0);
void draw_elliptic_outgon_P3D(Point left_top, Point right_bottom, int n, int rot = 0);
void draw_elliptic_outgon_P3D_filled(int x, int y, int rx, int ry, int n, int rot = 0);
void draw_elliptic_outgon_P3D_filled(Point center, int rx, int ry, int n, int rot = 0);
void draw_elliptic_outgon_P3D_filled(Point left_top, Point right_bottom, int n, int rot = 0);

//Drawing input_text
std::string draw_input_text(int x, int y, std::string prompt = "");
std::string draw_input_text(Point p, std::string prompt = "");
std::string draw_input_text_3D(int x, int y, std::string prompt = "");
std::string draw_input_text_3D(Point p, std::string prompt = "");
std::string draw_input_text_P3D(int x, int y, std::string prompt = "");
std::string draw_input_text_P3D(Point p, std::string prompt = "");

//Drawing lines
void draw_line(int x1, int y1, int x2, int y2);
void draw_line(Point p, int x2, int y2);
void draw_line(int x1, int y1, Point q);
void draw_line(Point p, Point q);

void draw_line_3D(int x1, int y1, int x2, int y2);
void draw_line_3D(Point p, int x2, int y2);
void draw_line_3D(int x1, int y1, Point q);
void draw_line_3D(Point p, Point q);

void draw_line_P3D(int x1, int y1, int x2, int y2);
void draw_line_P3D(Point p, int x2, int y2);
void draw_line_P3D(int x1, int y1, Point q);
void draw_line_P3D(Point p, Point q);

//Drawing ngons
//An ngon is a regular polygon inscribed in a circle.
//The default orientation has a bottom edge horizontal.
//We can optionally rotate from this default by "rot" degrees.
//See also outgons for regular polygons circumscribed about a circle.

void draw_ngon(int x, int y, int r, int n, int rot = 0);
void draw_ngon(Point center, int r, int n, int rot = 0);
void draw_ngon_filled(int x, int y, int r, int n, int rot = 0);
void draw_ngon_filled(Point center, int r, int n, int rot = 0);

void draw_ngon_3D(int x, int y, int r, int n, int rot = 0);
void draw_ngon_3D(Point center, int r, int n, int rot = 0);
void draw_ngon_3D_filled(int x, int y, int r, int n, int rot = 0);
void draw_ngon_3D_filled(Point center, int r, int n, int rot = 0);

void draw_ngon_P3D(int x, int y, int r, int n, int rot = 0);
void draw_ngon_P3D(Point center, int r, int n, int rot = 0);
void draw_ngon_P3D_filled(int x, int y, int r, int n, int rot = 0);
void draw_ngon_P3D_filled(Point center, int r, int n, int rot = 0);

//Drawing outgons
//An outgon is a regular polygon circumscribed about a circle.
//The default orientation has a bottom edge horizontal.
//We can optionally rotate from this default by "rot" degrees.
//See also ngons for regular polygons inscribed in a circle.
void draw_outgon(int x, int y, int r, int n, int rot = 0);
void draw_outgon(Point center, int r, int n, int rot = 0);
void draw_outgon_filled(int x, int y, int r, int n, int rot = 0);
void draw_outgon_filled(Point center, int r, int n, int rot = 0);

void draw_outgon_3D(int x, int y, int r, int n, int rot = 0);
void draw_outgon_3D(Point center, int r, int n, int rot = 0);
void draw_outgon_3D_filled(int x, int y, int r, int n, int rot = 0);
void draw_outgon_3D_filled(Point center, int r, int n, int rot = 0);

void draw_outgon_P3D(int x, int y, int r, int n, int rot = 0);
void draw_outgon_P3D(Point center, int r, int n, int rot = 0);
void draw_outgon_P3D_filled(int x, int y, int r, int n, int rot = 0);
void draw_outgon_P3D_filled(Point center, int r, int n, int rot = 0);

//Drawing points
void draw_point(int x, int y);
void draw_point(Point center);

void draw_point_3D(int x, int y);
void draw_point_3D(Point center);

void draw_point_P3D(int x, int y);
void draw_point_P3D(Point center);

//Drawing polygons
/*First declare an array of integer coordinates or Points as follows:
	int   vertcoords[] = {x1,y1, x2,y2, ...};
	Point vertpoints[] = {p1, p2, p3,...};
Then call a macro which calls the appropriate draw_polygon_f function:
	draw_polygon(vertcoords);
	draw_polygon(vertpoints);*/

#define draw_polygon(a) draw_polygon_f((a), sizeof(a)/sizeof(a[0])/  \
                     (sizeof(a[0])==sizeof(int)?2:1))
#define draw_polygon_filled(a) draw_polygon_filled_f((a), sizeof(a)/sizeof(a[0])/  \
                     (sizeof(a[0])==sizeof(int)?2:1))
#define draw_polygon_3D(a) draw_polygon_3D_f((a), sizeof(a)/sizeof(a[0])/  \
                     (sizeof(a[0])==sizeof(int)?2:1))
#define draw_polygon_3D_filled(a) draw_polygon_3D_filled_f((a), sizeof(a)/sizeof(a[0])/  \
                     (sizeof(a[0])==sizeof(int)?2:1))
#define draw_polygon_P3D(a) draw_polygon_P3D_f((a), sizeof(a)/sizeof(a[0])/  \
                     (sizeof(a[0])==sizeof(int)?2:1))
#define draw_polygon_P3D_filled(a) draw_polygon_P3D_filled_f((a), sizeof(a)/sizeof(a[0])/  \
                     (sizeof(a[0])==sizeof(int)?2:1))

void draw_polygon_f(Point* vertex_array, int ns);
void draw_polygon_f(int* coord_array, int ns);
void draw_polygon_f(VariablePoint* vertex_array, int ns);
void draw_polygon_filled_f(Point* vertex_array, int ns);
void draw_polygon_filled_f(int* coord_array, int ns);
void draw_polygon_filled_f(VariablePoint* vertex_array, int ns);
void draw_polygon_3D_f(Point* vertex_array, int ns);
void draw_polygon_3D_f(int* coord_array, int ns);
void draw_polygon_3D_f(VariablePoint* vertex_array, int ns);
void draw_polygon_3D_filled_f(Point* vertex_array, int ns);
void draw_polygon_3D_filled_f(int* coord_array, int ns);
void draw_polygon_3D_filled_f(VariablePoint* vertex_array, int ns);
void draw_polygon_P3D_f(Point* vertex_array, int ns);
void draw_polygon_P3D_f(int* coord_array, int ns);
void draw_polygon_P3D_f(VariablePoint* vertex_array, int ns);
void draw_polygon_P3D_filled_f(Point* vertex_array, int ns);
void draw_polygon_P3D_filled_f(int* coord_array, int ns);
void draw_polygon_P3D_filled_f(VariablePoint* vertex_array, int ns);

//Drawing polylines
/*First declare an array of integer coordinates or Points as follows:
	int vertcoords[] = {x1,y1,x2,y2,...};
	Point vertpoints[] = {p1, p2, p3,...};
Then call a macro which calls the appropriate draw_polyline_f function:
	draw_polyline(vertcoords);
	draw_polyline(vertpoints);*/

#define draw_polyline(a) draw_polyline_f(a, sizeof(a)/sizeof(a[0])/(sizeof(a[0])==sizeof(int)?2:1))
#define draw_polyline_3D(a) draw_polyline_3D_f(a, sizeof(a)/sizeof(a[0])/(sizeof(a[0])==sizeof(int)?2:1))
#define draw_polyline_P3D(a) draw_polyline_P3D_f(a, sizeof(a)/sizeof(a[0])/(sizeof(a[0])==sizeof(int)?2:1))

void draw_polyline_f(Point* vertex_array, int num_points);
void draw_polyline_f(int* coord_array, int num_points);
void draw_polyline_f(VariablePoint* vertex_array, int num_points);
void draw_polyline_3D_f(Point* vertex_array, int num_points);
void draw_polyline_3D_f(int* coord_array, int num_points);
void draw_polyline_3D_f(VariablePoint* vertex_array, int num_points);
void draw_polyline_P3D_f(Point* vertex_array, int num_points);
void draw_polyline_P3D_f(int* coord_array, int num_points);
void draw_polyline_P3D_f(VariablePoint* vertex_array, int num_points);

//Drawing rectangles
void draw_rectangle(Point left_top, Point right_bottom, int rot = 0);
void draw_rectangle(int x, int y, int rx, int ry, int rot = 0);
void draw_rectangle(Point center, int rx, int ry, int rot = 0);

void draw_rectangle_filled(Point left_top, Point right_bottom, int rot = 0);
void draw_rectangle_filled(int x, int y, int rx, int ry, int rot = 0);
void draw_rectangle_filled(Point center, int rx, int ry, int rot = 0);

void draw_rectangle_3D(Point left_top, Point right_bottom, int rot = 0);
void draw_rectangle_3D(int x, int y, int rx, int ry, int rot = 0);
void draw_rectangle_3D(Point center, int rx, int ry, int rot = 0);

void draw_rectangle_3D_filled(Point left_top, Point right_bottom, int rot = 0);
void draw_rectangle_3D_filled(int x, int y, int rx, int ry, int rot = 0);
void draw_rectangle_3D_filled(Point center, int rx, int ry, int rot = 0);

void draw_rectangle_P3D(Point left_top, Point right_bottom, int rot = 0);
void draw_rectangle_P3D(int x, int y, int rx, int ry, int rot = 0);
void draw_rectangle_P3D(Point center, int rx, int ry, int rot = 0);

void draw_rectangle_P3D_filled(Point left_top, Point right_bottom, int rot = 0);
void draw_rectangle_P3D_filled(int x, int y, int rx, int ry, int rot = 0);
void draw_rectangle_P3D_filled(Point center, int rx, int ry, int rot = 0);

//Function used by the draw_button package.
void draw_rounded_rectangle(Point left_top, Point right_bottom, 
								   int widthPercent, int heightPercent);

//Drawing squares
void draw_square(int x, int y, int r, int rot = 0);
void draw_square(Point center, int r, int rot = 0);
void draw_square_filled(int x, int y, int r, int rot = 0);
void draw_square_filled(Point center, int r, int rot = 0);

void draw_square_3D(int x, int y, int r, int rot = 0);
void draw_square_3D(Point center, int r, int rot = 0);
void draw_square_3D_filled(int x, int y, int r, int rot = 0);
void draw_square_3D_filled(Point center, int r, int rot = 0);

void draw_square_P3D(int x, int y, int r, int rot = 0);
void draw_square_P3D(Point center, int r, int rot = 0);
void draw_square_P3D_filled(int x, int y, int r, int rot = 0);
void draw_square_P3D_filled(Point center, int r, int rot = 0);

//Drawing text
void draw_text(int x, int y, std::string message);
void draw_text(Point p, std::string message);
void draw_text(int x, int y, char ch);
void draw_text(Point p, char ch);
void draw_text(int x, int y, int n);
void draw_text(Point p, int n);
void draw_text(int x, int y, unsigned int n);
void draw_text(Point p, unsigned int n);
void draw_text(int x, int y, double n);
void draw_text(Point p, double n);
void draw_text_3D(int x, int y, std::string message);
void draw_text_3D(Point p, std::string message);
void draw_text_3D(int x, int y, char ch);
void draw_text_3D(Point p, char ch);
void draw_text_3D(int x, int y, int n);
void draw_text_3D(Point p, int n);
void draw_text_3D(int x, int y, unsigned int n);
void draw_text_3D(Point p, unsigned int n);
void draw_text_3D(int x, int y, double n);
void draw_text_3D(Point p, double n);
void draw_text_P3D(int x, int y, std::string message);
void draw_text_P3D(Point p, std::string message);
void draw_text_P3D(int x, int y, char ch);
void draw_text_P3D(Point p, char ch);
void draw_text_P3D(int x, int y, int n);
void draw_text_P3D(Point p, int n);
void draw_text_P3D(int x, int y, unsigned int n);
void draw_text_P3D(Point p, unsigned int n);
void draw_text_P3D(int x, int y, double n);
void draw_text_P3D(Point p, double n);
///////////////////////////////////////////////////////////////////////////////
Point ellipse_point(Point left_top, Point right_bottom, int angle, int rot = 0);
Point ellipse_point(int x, int y, int rx, int ry, int angle, int rot = 0);
Point ellipse_point(Point center, int rx, int ry, int angle, int rot = 0);
Point ellipse_point(Point left_top, Point right_bottom, Point ray_point, int rot = 0);
Point ellipse_point(int x, int y, int rx, int ry, Point ray_point, int rot = 0);
Point ellipse_point(Point center, int rx, int ry, Point ray_point, int rot = 0);

void erase();
void erase(Color color);			//Changes current background color
void erase(int r, int g, int b);	//Changes current background color
void erase(int x, int y, int rx, int ry);
void erase(Point center, int rx, int ry);
void erase(Point left_top, Point right_bottom);

void erase_buffer();
void erase_buffer(Color color);
void erase_buffer(int r, int g, int b);
void erase_buffer(int x, int y, int rx, int ry);
void erase_buffer(Point center, int rx, int ry);
void erase_buffer(Point left_top, Point right_bottom);

void erase_buffer2();
void erase_buffer2(Color color);
void erase_buffer2(int r, int g, int b);
void erase_buffer2(int x, int y, int rx, int ry);
void erase_buffer2(Point center, int rx, int ry);
void erase_buffer2(Point left_top, Point right_bottom);

void flood_fill(int x, int y);
void flood_fill(Point fillPt);
void flood_fill(int x, int y, Color border_color);
void flood_fill(Point fillPt, Color border_color);
void flood_fill(int x, int y, int r, int g, int b);
void flood_fill(Point fillPt, int r, int g, int b);

bool is_char();
bool is_click();

int   get_3D_angle_x();
int   get_3D_angle_y();
Color get_3D_color();
int	  get_3D_depth();
int   get_3D_flare();
int	  get_3D_spacing();
int   get_3D_twist();
Point get_3D_vanishing_point();
int get_active_page();
int get_animation_easing();
bool get_animation_erase();
int get_animation_frames_per_second();
bool get_animation_repeat();
Color get_background_color();
char  get_char(bool wait = true, bool remove_prior = false, bool remove_returned = true);
Point get_click(bool wait = true, bool remove_prior = false, bool remove_returned = true);
Color get_color(int x, int y);
Color get_color(Point p);
Color get_fill_color();
Color get_pen_color();
int get_pen_style();
int get_pen_width();
Color get_pixel_color();
int get_shape_sides();
int  get_text_alignment();
Color get_text_color();
std::string get_text_font();
int get_text_rotation();
int get_text_size();
//See also text_height() and text_width()
//		 screen_height() and screen_width()

template <typename T>
int message_box(T message, std::string caption = "")
{
	return message_box(to_string(message), caption);
}

int message_box(std::string message, int buttons = DEFAULT_MB, int default_button = DEFAULT_MB, int icon = DEFAULT_MB);
int message_box(std::string message, std::string caption, int buttons = DEFAULT_MB, int default_button = DEFAULT_MB, int icon = DEFAULT_MB);

void open_window(int width = -1, int height = -1, std::string caption = "GrafiX");
void open_window(std::string caption);

bool point_in_rectangle(Point left_top, Point right_bottom, Point p);
bool point_in_rectangle(int x, int y, int rx, int ry, Point p);
bool point_in_rectangle(Point center, int rx, int ry, Point p);

void play_sound(std::string sound, bool loop = false);
void play_bundled_sound(std::string sound_name, bool loop = false);
void quit(bool close_window_immediately = true);
void quit_confirm(bool close_window_immediately = true);
int   random(int high);			//from 1 to high
int   random(int low, int high);
Color random_blue (int low = 0, int high = 255);
Color random_color();
Color random_color(Color low, Color high);
double random_continuous(double low, double high, double increment = .01);
Color random_green(int low = 0, int high = 255);
Point random_point();
Point random_point(int x, int y, int rx, int ry);
Point random_point(Point center, int rx, int ry);
Point random_point(Point left_top, Point right_bottom);
Point random_point(int inset_x, int inset_y);
Color random_red  (int low = 0, int high = 255);
RoundedShape random_rounded_shape(int x_min = PerX(25), int x_max = PerX(75),
				   int y_min = PerY(25), int y_max = PerY(75),
				   int rx_min = PerX(5), int rx_max = PerX(35),
				   int ry_min = PerY(5), int ry_max = PerY(35),
				   int ns_min = 3, int ns_max = 8,
				   int out_min = -100, int out_max = 100);
RoundedShape random_rounded_shape(RoundedShape lim1, RoundedShape lim2);
Shape random_shape(int x_min = PerX(25), int x_max = PerX(75),
				   int y_min = PerY(25), int y_max = PerY(75),
				   int rx_min = PerX(5), int rx_max = PerX(35),
				   int ry_min = PerY(5), int ry_max = PerY(35),
				   int ns_min = 3, int ns_max = 8,
				   int rot_min = 0, int rot_max = 360, 
				   int out_min = -100, int out_max = 100);
Shape random_shape(Shape lim1, Shape lim2);
int   random_x();
int   random_y();

Color range(Color  start, Color end, double proportion = InCurrentAnimation);
int   range(int    start, int   end, double proportion = InCurrentAnimation);
Point range(Point  start, Point end, double proportion = InCurrentAnimation);
RoundedShape range(RoundedShape start, RoundedShape stop, double proportion = InCurrentAnimation);
Shape range(Shape  start, Shape end, double proportion = InCurrentAnimation);

//This template should range over the passed array for any type 
//that has a "T range(T start, T stop)" function.
template <typename T>
T range(T* array, int size, double proportion = InCurrentAnimation)
{
	int hi = size - 1;
	if(proportion >= 1.0)
		return array[hi];
	int i = int(proportion * hi);  //i is the index of the interval's lower endpoint.
	double new_prop = proportion * hi - i;
	return range(array[i], array[i+1], new_prop);
}
template <typename T>
T range(std::vector<T> vec, double proportion = InCurrentAnimation)
{
	int hi = vec.size() - 1;
	if(proportion >= 1.0)
		return vec[hi];
	int i = int(proportion * hi);  //i is the index of the interval's lower endpoint.
	double new_prop = proportion * hi - i;
	return range(vec[i], vec[i+1], new_prop);
}

double range_continuous(double start, double end, double proportion = InCurrentAnimation);


template <typename T>
T range_cycle(T start, T extreme, int num_cycles = 1, double proportion = InCurrentAnimation)
{
	double i = proportion * (2 * num_cycles);
	if(int(i) % 2 == 0)
		return range(start, extreme, i - int(i));
	else
		return range(extreme, start, i - int(i));
}
template <typename T>
T range_delay(T start, T end, int start_percent, int stop_percent, double proportion = InCurrentAnimation)
{
	if(proportion <= start_percent / 100.0) return start;
	else if(proportion > stop_percent / 100.0) return end;
	else return range(start, end, (100.0 * proportion - start_percent) / (stop_percent - start_percent));
}
template <typename T>
T range_delay(T before, T start, T end, T after, int start_percent, int stop_percent, double proportion = InCurrentAnimation)
{

	if(proportion <= start_percent / 100.0) return before;
	else if(proportion > stop_percent / 100.0) return after;
	else return range(start, end, (100.0 * proportion - start_percent) / (stop_percent - start_percent));
}
template <typename T>
T range_repeat(T start, T extreme, int num_reps = 1, double proportion = InCurrentAnimation)
{
	double i = proportion * num_reps;
	return range(start, extreme, i - int(i));
}
void remove_char();
void remove_click();
void rotate(Point* point_array, int num_points, int x, int y, int angle);
void rotate(Point* point_array, int num_points, Point center, int angle);
Point rotate(Point p, int x, int y, int angle);
Point rotate(Point p, Point center, int angle);

int round(double);
int run_time();

void set_3D_angle_x(int t);
void set_3D_angle_y(int t);
void set_3D_color(Color c);
void set_3D_color(int r, int g, int b);
void set_3D_depth (int n);
void set_3D_flare(int n);
void set_3D_spacing(int n);
void set_3D_twist(int twist);
void set_3D_vanishing_point (Point p);
void set_active_page(int page);
void set_animation_easing(int easing);
void set_animation_erase(bool status);
void set_animation_frames_per_second(int frames_per_second);
void set_animation_repeat(bool repeat_status);
void set_background_color(Color color);
void set_background_color(int r, int g, int b);
void set_color(Color color);
void set_color(int r, int g, int b);
void set_fill_color(Color);
void set_fill_color(int, int, int);
void set_pen_color(Color pen_color);
void set_pen_color(int r, int g, int b);
void set_pen_style(int pen_style_enum);
void set_pen_width(int pen_point_size);
void set_pixel_color(Color color);
void set_pixel_color(int r, int g, int b);
void set_shape_sides(int num_sides);
void set_text_alignment(int align_enum);
void set_text_color(Color color);
void set_text_color(int r, int g, int b);
void set_text_font(std::string font_name = "");	//Default gives system font
void set_text_rotation(int angle);
void set_text_size(int points);

//Note that these are given a foward declaration with MaxPt
//int screen_height();
//int screen_width();

//reset puts back defaults, save takes a snapshot, 
//and restore puts back snapshot or defaults
void settings_3D_reset();
void settings_3D_restore();
void settings_3D_save();

void settings_animation_reset();
void settings_animation_restore();
void settings_animation_save();

void settings_fill_reset();
void settings_fill_restore();
void settings_fill_save();

void settings_pen_reset();
void settings_pen_restore();
void settings_pen_save();

void settings_reset();
void settings_restore();
void settings_save();

void settings_text_reset();
void settings_text_restore();
void settings_text_save();

void stop_sound();

int text_height(std::string s);
int text_width(std::string s);

double to_double(std::string);
int to_int(std::string);
int to_int(char);
int to_int(double);
std::string to_string(const char* str);
std::string to_string(char ch);
std::string to_string(int n);
std::string to_string(unsigned int n);
std::string to_string(double n);

//The passed array should have at least ns size.
void vertices_elliptic_ngon(Point* vertex_array, int x, int y, int rx, int ry, int ns, int rot = 0);
void vertices_elliptic_ngon(Point* vertex_array, Point center, int rx, int ry, int ns, int rot = 0);
void vertices_elliptic_outgon(Point* vertex_array, int x, int y, int rx, int ry, int ns, int rot = 0);
void vertices_elliptic_outgon(Point* vertex_array, Point center, int rx, int ry, int ns, int rot = 0);
void vertices_ngon(Point* vertex_array, int x, int y, int r, int ns, int rot = 0);
void vertices_ngon(Point* vertex_array, Point center, int r, int ns, int rot = 0);
void vertices_outgon(Point* vertex_array, int x, int y, int r, int ns, int rot = 0);
void vertices_outgon(Point* vertex_array, Point center, int r, int ns, int rot = 0);

void GrafiX_Welcome();

#endif	//End of the files include guards to prevent multiple inclusions.