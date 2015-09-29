//If you wish to write a GrafiX program, close this file and open MyCode.cpp instead.

//******************************************************************************
//**************************GrafiX.cpp VERSION 4.5.05***************************
//For use with MS Visual Studio 2008 Express
//******************************************************************************
/*
	GrafiX is a framework for doing graphics and animation using standard C++
	within a Windows environment.  It was developed by Dr. Erlan Wheeler of 
	Carthage College. (ewheeler.carthage.edu).  Visit the GrafiX site at
	http://www.carthage/ewheeler/grafix to learn more.
*/

#include "grafiX.h"
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <TCHAR.H>
using std::copy;

#pragma comment(lib, "winmm.lib") //Windows MultiMedia lib for sound
/*To generate executables that others can run, do a Release Build
with Properties->Config Props->c++->Code Generation->Runtime Lib /MT.
Then don't trash the hidden .suo file.*/

static const double PI = 3.14159265358979323846;

////////////////////////////////////////////////////////////////////////////////
//////////////////STATIC VARIABLES USED BY THIS FILE ONLY///////////////////////
////////////////////////////////////////////////////////////////////////////////
//Background Properties
static Color	gfx_background_color_default = BLACK,
				gfx_background_color, gfx_background_color_save;

//Fill Properties
static Color		gfx_fill_color_default = WHITE,
					gfx_fill_color,	gfx_fill_color_save;

//Pen Properties
static Color		gfx_pen_color_default = WHITE,
					gfx_pen_color, gfx_pen_color_save;
static UINT			gfx_pen_style_default = SOLID_PEN,
					gfx_pen_style, gfx_pen_style_save;
static int			gfx_pen_width_default = 1,
					gfx_pen_width, gfx_pen_width_save;

//Pixel Properties
static Color		gfx_pixel_color_default = WHITE,
					gfx_pixel_color, gfx_pixel_color_save;

//Text Properties
static int			gfx_text_alignment_default = CENTER,
					gfx_text_alignment, gfx_text_alignment_save;
static Color		gfx_text_color_default = WHITE,
					gfx_text_color,	gfx_text_color_save;
static std::string	gfx_text_font_default = COURIER,
					gfx_text_font, gfx_text_font_save;
static int			gfx_text_rotation_default = 0,
					gfx_text_rotation, gfx_text_rotation_save;
static int			gfx_text_size_default = 12,
					gfx_text_size, gfx_text_size_save;

//3D Properties
int					gfx_3D_angle_x_default = -45,
					gfx_3D_angle_x,	gfx_3D_angle_x_save;
int					gfx_3D_angle_y_default = -45,
					gfx_3D_angle_y, gfx_3D_angle_y_save;
Color				gfx_3D_color_default = RED,
					gfx_3D_color, gfx_3D_color_save;
int					gfx_3D_depth_default = 100,
					gfx_3D_depth, gfx_3D_depth_save;
int					gfx_3D_flare_default = 0,
					gfx_3D_flare, gfx_3D_flare_save;
int					gfx_3D_spacing_default = 1,
					gfx_3D_spacing,	gfx_3D_spacing_save;
int					gfx_3D_twist_default = 0,
					gfx_3D_twist, gfx_3D_twist_save;
Point				gfx_3D_vanishing_point_default = Point (400, 300),	//This will be changed...
					gfx_3D_vanishing_point,	gfx_3D_vanishing_point_save;

static int			gfx_active_page_default = 0,
                    gfx_active_page, gfx_active_page_save;	//Currently active page (0-3)

//Animator Properties
Animator gfx_animation;	//our global Animator object
Animator* Animator::current = &gfx_animation;
int		  gfx_animation_easing_default = 0,	//Positive values make it slow over time, negatives to speed up over time.
		  gfx_animation_easing, gfx_animation_easing_save;
bool      gfx_animation_erase_default = true,
          gfx_animation_erase, gfx_animation_erase_save;
int       gfx_animation_frames_per_second_default = 30;
int       gfx_animation_frames_per_second = gfx_animation_frames_per_second_default, //needed in set_animation_fps
		  gfx_animation_frames_per_second_save;
bool      gfx_animation_repeat_default = false,
          gfx_animation_repeat, gfx_animation_repeat_save;

//Mouse Location and Last Character
static int gfx_current_mouse_y;
static int gfx_current_mouse_x;
static Point gfx_last_click_point = Point(-1,-1);
static char gfx_last_char = 0;

//Environment Variables
static const int gfxe_NUMPAGES = 4;
static HWND gfxe_hwnd = NULL;				//Handle to the GrafiX window
static HDC gfxe_hdc[gfxe_NUMPAGES];				//Device contexts for visible screen (0),
									//animation(1), background(2), & copy buffers(3)
static RECT gfxe_screen_rectangle;		//The client area rectangle
static HBRUSH gfxe_background_brush;	//Used to paint the background

static bool gfxe_close_flag = false;	//Should window close upon program termination
static bool gfxe_program_done = false;	//Needed for when user presses closebox after
	//program is done and just displaying the window.
static int gfxe_begin_time;

static double DEG2RAD = PI / 180.0;
static double RAD2DEG = 180.0 / PI;

////////////////////////////////////////////////////////////////////////////////
//////////////////STATIC FUNCTIONS USED BY THIS FILE ONLY///////////////////////
////////////////////////////////////////////////////////////////////////////////
//MESSAGES FROM WINDOWS ARE PROCESSED BY THIS WINDOW PROCEDURE//////////////////
static long CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int i;
	switch(msg)
	{
	case WM_CREATE:		//The CreateWindow() function was just called
		gfxe_hdc[0] = GetDC( hWnd );

		  //I don't think that we need this?
		  PIXELFORMATDESCRIPTOR pfd;
		  ZeroMemory( &pfd, sizeof pfd );
		  pfd.nSize = sizeof pfd;
		  pfd.nVersion = 1;
		  pfd.dwFlags = PFD_DRAW_TO_WINDOW;
		  pfd.iPixelType = PFD_TYPE_RGBA;
		  pfd.cColorBits = 24;
  		  i = ChoosePixelFormat( gfxe_hdc[0], &pfd );  
		  SetPixelFormat( gfxe_hdc[0], i, &pfd );

		return 0;

	case WM_PAINT:		//The window needs to be redrawn in part or in whole
		//copy the "copy buffer" to the visible screen
		BitBlt(gfxe_hdc[0], 0,0, screen_width(), screen_height(), gfxe_hdc[3], 0, 0, SRCCOPY);
		//Validate the screen to stop further WM_PAINT messages
		//This is typically done with a BeginPaint/EndPaint pair.
		ValidateRect(gfxe_hwnd,NULL);
		return 0;
	
	case WM_CHAR:
		//Save the ASCII code of the pressed key
		gfx_last_char = (char) wParam;
		return 0;

	case WM_CLOSE:		//User clicked the close box of the GrafiX window.
		quit_confirm(true);	//close window if user wants to quit
		return 0;				//User does not want to quit after all

	case WM_DESTROY:		  //The window is about to be closed
		PostQuitMessage(0);	  //Post a WM_QUIT message to terminate message loop
		return 0;

	case WM_LBUTTONDOWN:
		gfx_last_click_point = Point(LOWORD(lParam), HIWORD(lParam));
		return 0;

	case WM_SIZE:		//ShowWindow() called or resize operation
		//Store the new size of the window's client area in a RECT structure
		GetClientRect(hWnd, &gfxe_screen_rectangle);
        return 0;

	case WM_USER:
	        gfx_current_mouse_y = (int) ((lParam >> 16) & 0xffff);
            gfx_current_mouse_x = (int) (lParam & 0xffff);
		return 0;
	}	//End of switch(msg)

	//Call the default window procedure for other messages
	return (long) DefWindowProc(hWnd, msg, wParam, lParam);
}

////////////SET UP DEFAULTS AND ALLOCATE GRAFIX RESOURCES///////////////////////
static void InitializeGrafiX()
{
	//Initialize the pen which draws lines.
	HPEN hpen = CreatePen(gfx_pen_style,			//pen_style
						  gfx_pen_width,			//pen thickness
						  RGB(gfx_pen_color.r,gfx_pen_color.g,gfx_pen_color.b));			//pen color
			
	//This will replace the Stock brush which we don't want to delete
	{
		Color c = gfx_background_color;
		gfxe_background_brush = CreateSolidBrush(RGB(c.r,c.g,c.b));
	}

	//Create memory device contexts for the buffers and a blank bitmap for each
	//1) buffer for changing animation
	//2) buffer for static background
	//3) buffer which saves a copy of the visible screen
	int c;
	for(c = 1; c < gfxe_NUMPAGES; c++)
	{
		gfxe_hdc[c] = CreateCompatibleDC(gfxe_hdc[0]);
		SelectObject(gfxe_hdc[c], CreateCompatibleBitmap(
														gfxe_hdc[0], 
														screen_width(), 
														screen_height())
													  );
	}
	erase();

   
	//Initialize the brush which fills solid areas
	//int brush_pattern = SOLID_FILL;
	HBRUSH hbrush = CreateSolidBrush(RGB(gfx_fill_color.r,gfx_fill_color.g,gfx_fill_color.b));	//brush color

#ifdef _UNICODE
	//The following is needed because the project default is Unicode:
	//Project -> Properties -> Configuration -> General -> Character Set
	TCHAR* unicodePtr = new TCHAR[gfx_text_font_default.length() + 1];
	for(int i = 0; i < (int) gfx_text_font_default.length(); i++)
		unicodePtr[i] = gfx_text_font_default[i];
	unicodePtr[gfx_text_font_default.length()] = '\0';

	//All values set to zero will be given default values by Windows
	HFONT hfont = CreateFont
	(
		  0,        // logical height of font -- this is not the point size
		  0,         // logical average character width
		  0,         // angle of escapement
		  0,        // base-line orientation angle
		  0,        // font weight
		  0,        // italic attribute flag
		  0,		// underline attribute flag
		  0,		// strikeout attribute flag
		  DEFAULT_CHARSET,  // character set identifier
		  0,		// output precision
		  0,		// clipping precision
		  0,        // output quality
		  0,		// pitch and family
		  (LPCWSTR) unicodePtr   // pointer to typeface name string
	);
	delete[] unicodePtr;
#else
	HFONT hfont = CreateFont
	(
		  0,        // logical height of font -- this is not the point size
		  0,         // logical average character width
		  0,         // angle of escapement
		  0,        // base-line orientation angle
		  0,        // font weight
		  0,        // italic attribute flag
		  0,		// underline attribute flag
		  0,		// strikeout attribute flag
		  DEFAULT_CHARSET,  // character set identifier
		  0,		// output precision
		  0,		// clipping precision
		  0,        // output quality
		  0,		// pitch and family
		  gfx_text_font_default.c_str()   // pointer to typeface name string
	);
#endif
 
	//Select the pen and brush and font into the device contexts
	for(c = 0; c < gfxe_NUMPAGES; c++)
	{
		SelectObject(gfxe_hdc[c], hpen);
		SelectObject(gfxe_hdc[c], hbrush);
		SelectObject(gfxe_hdc[c], hfont);
		FillRect(gfxe_hdc[c], &gfxe_screen_rectangle, gfxe_background_brush);
		SetTextColor(gfxe_hdc[c], RGB(gfx_text_color.r,gfx_text_color.g,gfx_text_color.b));
		SetBkColor(gfxe_hdc[c], RGB(BLUE.r,BLUE.g,BLUE.b));
		SetBkMode(gfxe_hdc[c], TRANSPARENT);
	}

	gfx_3D_vanishing_point_default = Point(MidX, int(.33 * MaxY));

	settings_reset();
	settings_save();

	for(c = gfxe_NUMPAGES - 1; c >= 0; c--)
	{
		set_active_page(c);
		erase();
	}	//Note that 0 is now the active page.
}

//******************************************************************************
//**********************THE COLOR CLASS*****************************************
//******************************************************************************
Color::Color(int red, int green, int blue)
{
	if(red <= 0) r = 0;
	else if (red >= 255) r = 255;
	else r = red;

	if(green <= 0) g = 0;
	else if (green >= 255) g = 255;
	else g = green;

	if(blue <= 0) b = 0;
	else if (blue >= 255) b = 255;
	else b = blue;
}
Color Color::dark(int percent)
{
	if(percent >= 100) 
		return BLACK;
	else if(percent < 0) 
		return light(- percent);
	else
		return Color(r * (100 - percent) / 100,
			         g * (100 - percent) / 100,
				     b * (100 - percent) / 100);
}
Color Color::light(int percent)
{
	if(percent >= 100) 
		return WHITE;
	else if(percent < 0) 
		return dark(- percent);
	else
		return Color((r * (100 - percent) + 255 * percent) / 100,
		             (g * (100 - percent) + 255 * percent) / 100,
				     (b * (100 - percent) + 255 * percent) / 100);
}
Color Color::inverse()
{
	return Color(255 - r, 255 - g, 255 - b);
}
bool operator==(Color& lhs, Color& rhs)
{
	return
	GetNearestColor(gfxe_hdc[0], RGB(lhs.r, lhs.g, lhs.b)) == 
	GetNearestColor(gfxe_hdc[0], RGB(rhs.r, rhs.g, rhs.b));
}
bool operator!=(Color& lhs, Color& rhs)
{
	return !(lhs == rhs);
}
//*****************************************************************************
//*************************THE Point CLASS*************************************
//*****************************************************************************

/*Default constructor.  You might think that you can get rid of this 
one and just put default arguments to the other constructor.  If you do this, 
I find that strange implicit conversions start occurring.*/
Point::Point()
:x(-1),y(-1)
{

}
Point::Point(int x_init, int y_init)
: x(x_init), y(y_init)
{

}
bool Point::operator==(Point rhs)
{
	return ((x == rhs.x) && (y == rhs.y));
}
bool Point::operator!=(Point rhs)
{
	return ((x != rhs.x) || (y != rhs.y));
}
Point Point::operator+(Point rhs)
{
	Point p = *this;
	p.x = x + rhs.x;
	p.y = y + rhs.y;
	return p;
}
Point Point::operator-(Point rhs)
{
	Point p = *this;
	p.x = x - rhs.x;
	p.y = y - rhs.y;
	return p;
}
Point Point::operator/(int rhs)
{
	return Point(x/rhs, y/rhs);
}
//*****************************************************************************
//*************************THE Animator CLASS**********************************
//*****************************************************************************
/*The default is to work no faster than 30 .animates() per second*/
Animator::Animator(double total_secs, bool repeat)
{
	if(total_secs < 0)
		total_secs = 0;

	active_status = false;
	set_animation_repeat(repeat);

	total_frames  = int(get_animation_frames_per_second() * total_secs);
	frame_on_buffer = 0;

	begin();
}
/*Returns true until we generate total_frames or "esc"*/
bool Animator::animate()
{
//This will allow the close and minimize to work.
//I still have some work to do on the "resize" button.
//I don't know if it slows things down too much.  GetMessage does...
	MSG msg;
	if(PeekMessage(&msg, NULL, 0,0,PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//Check for pause keypress (which doesn't generate a WM_CHAR
	//and so is not detectable by is_char()).  This function is 
	//complicate by the fact that we want the pause, home, end, 
	//and escape keys to be able to "unpause" with appropriate 
	//behavior in the latter cases.  For that reason, we need to 
	//check for pause before checking for the others.
	if(GetAsyncKeyState( VK_PAUSE ) )	//Pause animation
	{
		suspend();
		while(GetAsyncKeyState( VK_PAUSE ));
		//remove_char();
		while(true)		//Until the user indicates pausing is over
		{
			if(GetAsyncKeyState( VK_PAUSE ))
				while(GetAsyncKeyState (VK_PAUSE));	//unpause
			else if(GetAsyncKeyState( VK_END )  || //unpause and process accordingly
				    GetAsyncKeyState( VK_HOME ) ||
					GetAsyncKeyState( VK_ESCAPE ))	
				break;		//key is still pressed by the time later check occur below
			else if(is_char())	//unpause on key press
				remove_char();
			else if(is_click())	//unpause on key press
				remove_click();
			else
				continue;		//continue in loop otherwise
			break;				//This breaks the loop for unpause cases!
		}
		unsuspend();
	}
	else if(GetAsyncKeyState( VK_ESCAPE ))	//Terminate program
	{
		while(GetAsyncKeyState( VK_ESCAPE ));
		remove_char();
		end();
		quit(false);		//Terminate program but don't close window
		return false;
	}
	else if(GetAsyncKeyState( VK_END ))	//End just this animation
	{
		while(GetAsyncKeyState( VK_END ));
		end();
		return false;
	}
	else if(GetAsyncKeyState( VK_HOME ))	//Restart animation
	{
		while(GetAsyncKeyState( VK_HOME ));
		end();
		begin();
		return true;
	}
	//Check for first time
	if(frame_on_buffer == 0)
	{
		active_status = true;
		frame_on_buffer = 1;
		clock_begin_time = GetTickCount();
		suspend_begin_time = GetTickCount();

		set_active_page(BUFFER);
		erase();
		if(get_animation_erase() == false)
			copy_page(SCREEN, BUFFER);
		current = this;
		return true;
	}

	if(!active_status) return false;

	//Possibly halt processing if animation is proceeding too quickly.
	if(gfx_animation_easing == 0)
		while(1000 * frame_on_buffer > get_animation_frames_per_second() *(GetTickCount() - clock_begin_time));
	else
	{
		//The numbers get rather large, so we should work with doubles.
		double e = get_animation_easing();
		double f = get_animation_frames_per_second();
		double n = total_frames;
		double k = frame_on_buffer;

		double c = int(k / n);	//c is the cycle number for repeat: 0 on first cycle
		int kh = int(k) % int(n);			//kh is the frame in the current cycle

		//Slowing down
		if(gfx_animation_easing > 0)
			while( c * n / f + (kh+2) * (20*n+e*kh+e-20) / (f*(20+e)*(n-1)) > 
				  (GetTickCount() - clock_begin_time) / 1000.0 );
		//Speeding up
		else
		{
			while( c * n / f - (kh*(e*(1.0+kh-2.0*n)+20.0*(n-1))) / ((e-20.0)*f*(n-1.0)) > 
				  ((GetTickCount() - clock_begin_time) / 1000.0) );
		}
	}

	//Render
	copy_page(BUFFER, SCREEN);			//copy_page(1,0);

	if(!get_animation_repeat() && frame_on_buffer >= total_frames)
	{
		end();
		return false;
	}
	
	frame_on_buffer++;	//Set up for next pass through animate.

	if(get_animation_erase())
		erase();

	return true;
}
void Animator::begin()
{
	active_status = false;

	frame_on_buffer = 0;
	clock_begin_time = 0;
	suspend_begin_time = 0;

	while(GetAsyncKeyState( VK_ESCAPE ));	//In case the escape key IS down
	while(GetAsyncKeyState( VK_PAUSE ));    //In case the pause WAS pressed earlier
	while(GetAsyncKeyState( VK_END ));		//In case the end WAS pressed earlier
}
void Animator::end()
{
	active_status = false;
	frame_on_buffer = total_frames + 1;
	clock_begin_time = 0;
	suspend_begin_time = 0;
	current = &gfx_animation;
	set_active_page(SCREEN);
}
void Animator::suspend()
{
	if(active_status == false)
		return;
	active_status = false;
	suspend_begin_time = GetTickCount();
	set_active_page(SCREEN);
}
void Animator::unsuspend()
{
	if(active_status == true)
		return;
	active_status = true;
	clock_begin_time = clock_begin_time + (GetTickCount() - suspend_begin_time);
	suspend_begin_time = GetTickCount();
	set_active_page(BUFFER);
}
void Animator::set_timer(double total_secs)
{
	if(total_secs < 0)
		total_secs = 0;
	//total_msecs = int(1000 * total_secs);
	total_frames  = int(get_animation_frames_per_second() * total_secs);
}

bool Animator::is_active()
{
	return active_status;
}
Animator::operator double()
{
	return proportion();
}
double Animator::proportion()
{
	if(total_frames == 0)
		return 0;
	int mod = frame_on_buffer % total_frames;
	if(mod == 0)
		return 1.0;
	else
		return double(mod) / total_frames;
}
//*****************************************************************************
//****************************THE Grid CLASS***********************************
//*****************************************************************************

//Grid Constructors

Grid::Grid()
{
	(*this) = Grid(2,1);
}
Grid::Grid(int num_rows, int num_cols)
{
	(*this) = Grid(Point(0,0), Point(800,40), num_rows, num_cols);
}

Grid::Grid(int x, int y, int rx, int ry, int num_rows, int num_cols)
{
	*this = Grid(Point(x,y), rx, ry, num_rows, num_cols);
}

//This constructor may adjust the width and height so that each cell
//has an integral width and height.  The center is unchanged.
Grid::Grid(Point center, int rx, int ry, int num_rows, int num_cols)

	//Dimension properties
   :m_num_rows(num_rows > 0 ? num_rows : 2),
	m_num_cols(num_cols > 0 ? num_cols : 1),
	
	 //Cell reference properties
	m_first_index(0),
	m_first_row(0),
	m_first_col(0)
{
	if(rx < 0) rx = -rx;
	if(ry < 0) ry = -ry;

	//Adjusted position properties
	Point new_width_height = 
		Point(round(2.0 * rx / m_num_cols) * m_num_cols,
			  round(2.0 * ry / m_num_rows) * m_num_rows);
	m_left_top = (center - new_width_height / 2);
	m_right_bottom = m_left_top + new_width_height;
}

//This constructor may adjust the width and height so that each cell
//has an integral width and height.  The left_top is unchanged.
Grid::Grid(Point left_top_pt, Point right_bottom_pt, int num_rows, int num_cols)

	//Dimension properties
   :m_num_rows(num_rows > 0 ? num_rows : 2),
	m_num_cols(num_cols > 0 ? num_cols : 1),
	
	 //Cell reference properties
	m_first_index(0),
	m_first_row(0),
	m_first_col(0)
{
	//We cannot assume that left_top is really the left top...
	Point ltp = Point(min(left_top_pt.x, right_bottom_pt.x),
		              min(left_top_pt.y, right_bottom_pt.y));
	Point rbp = Point(max(left_top_pt.x, right_bottom_pt.x),
		              max(left_top_pt.y, right_bottom_pt.y));

	//Adjusted position properties
	double old_width = rbp.x - ltp.x;
	int new_width  = round(old_width / m_num_cols) * m_num_cols;
	double old_height = rbp.y - ltp.y;
	int new_height  = round(old_height / m_num_rows) * m_num_rows;
	m_left_top = ltp;
	m_right_bottom = ltp + Point(new_width, new_height);
}

//Construct Grid in rectangular region defined by another Grid/GridCell.
Grid::Grid(Grid grid, int num_rows, int num_cols)

	//Dimension members
   :m_num_rows(num_rows > 0 ? num_rows : 2),
	m_num_cols(num_cols > 0 ? num_cols : 1),

	//Cell reference properties
	m_first_index(0),
	m_first_row(0),
	m_first_col(0),

	//Position properties
	m_left_top(grid.m_left_top),
	m_right_bottom(grid.m_right_bottom)
{

}
//Obtaining GridCells

GridCell Grid::operator()(int index)
{
	return (*this)(index_to_row(index), index_to_col(index));
}
GridCell Grid::operator()(int row, int col)
{
	return GridCell(Point(m_left_top.x + (col - m_first_col) * 
		                   (m_right_bottom.x - m_left_top.x) / m_num_cols,
				          m_left_top.y + (row - m_first_row) * 
						   (m_right_bottom.y - m_left_top.y) / m_num_rows),
			        Point(m_left_top.x + (col - m_first_col + 1) *
					       (m_right_bottom.x - m_left_top.x) / m_num_cols,
		                  m_left_top.y + (row - m_first_row + 1) * 
						   (m_right_bottom.y - m_left_top.y) / m_num_rows),
				    row, col, rowcol_to_index(row, col));
}
GridCell Grid::cursor_cell()
{
	return point_to_cell(CurPt);
}
GridCell Grid::random_cell()
{
	return (*this)(random(first_index(), last_index()));
}


//Grid Drawing Functions

void Grid::draw(bool border, bool gridlines)
{
	if(border)
		draw_rectangle(m_left_top, m_right_bottom);

	if(!gridlines)
		return;

	double h = m_right_bottom.x - m_left_top.x;
	double v = m_right_bottom.y - m_left_top.y;

	//Draw vertical lines
	int c;
	for(c = 1; c < m_num_cols; c++)
		draw_line(int(m_left_top.x + c * h / m_num_cols), m_left_top.y,
		          int(m_left_top.x + c * h / m_num_cols), m_right_bottom.y);
	//Draw horizontal lines
	for(int r = 1; r < m_num_rows; r++)
		draw_line(m_left_top.x,  int(m_left_top.y + r * v / m_num_rows),
		          m_right_bottom.x, int(m_left_top.y + r * v / m_num_rows));
}
void Grid::draw(int inset_percent)
{
	draw(inset_percent, inset_percent);
}
void Grid::draw(int inset_percent_x, int inset_percent_y)
{
	for(int i = first_index(); i <= last_index(); i++)
		draw_rectangle(
		    (*this)(i).inset_left_top(inset_percent_x, inset_percent_y),
		    (*this)(i).inset_right_bottom(inset_percent_x, inset_percent_y));
}
void Grid::draw_circle()
{
	int radius = cell_radius_y();
	if(cell_radius_x() < radius)
		radius = cell_radius_x();
	for(int i = first_index(); i <= last_index(); i++)
		::draw_circle((*this)(i).center(), radius - 1);
}
void Grid::draw_circle(int inset_percent)
{
	draw_circle(inset_percent, inset_percent);
}
void Grid::draw_circle(int inset_percent_x, int inset_percent_y)
{
	int radius1 = cell_radius_x(inset_percent_x);
	int radius2 = cell_radius_y(inset_percent_y);
	if(radius1 < radius2)
		draw_ellipse(
		  inset_percent_x, 100 - 100 * radius1 / cell_radius_y());
	else
		draw_ellipse(
		  100 - 100 * radius2 / cell_radius_x(), inset_percent_y);
}
void Grid::draw_circle_filled()
{
	int radius = cell_radius_y();
	if(cell_radius_x() < radius)
		radius = cell_radius_x();
	for(int i = first_index(); i <= last_index(); i++)
		::draw_circle_filled((*this)(i).center(), radius - 1);
}
void Grid::draw_circle_filled(int inset_percent)
{
	draw_circle_filled(inset_percent, inset_percent);
}
void Grid::draw_circle_filled(int inset_percent_x, int inset_percent_y)
{
	int radius1 = cell_radius_x(inset_percent_x);
	int radius2 = cell_radius_y(inset_percent_y);
	if(radius1 < radius2)
		draw_ellipse_filled(
		  inset_percent_x, 100 - 100 * radius1 / cell_radius_y());
	else
		draw_ellipse_filled(
		  100 - 100 * radius2 / cell_radius_x(), inset_percent_y);
}
void Grid::draw_filled(int inset_percent)
{
	draw_filled(inset_percent, inset_percent);
}
void Grid::draw_filled(int inset_percent_x, int inset_percent_y)
{
	for(int i = first_index(); i <= last_index(); i++)
		draw_rectangle_filled(
		    (*this)(i).inset_left_top(inset_percent_x, inset_percent_y),
		    (*this)(i).inset_right_bottom(inset_percent_x, inset_percent_y));
}
void Grid::draw_ellipse()
{
	for(int i = first_index(); i <= last_index(); i++)
		::draw_ellipse((*this)(i).left_top() + Point(2,2),
		               (*this)(i).right_bottom() - Point(2,2));
}
void Grid::draw_ellipse(int inset_percent)
{
	draw_ellipse(inset_percent, inset_percent);
}
void Grid::draw_ellipse(int inset_percent_x, int inset_percent_y)
{
	for(int i = first_index(); i <= last_index(); i++)
		::draw_ellipse(
		    (*this)(i).inset_left_top(inset_percent_x, inset_percent_y),
		    (*this)(i).inset_right_bottom(inset_percent_x, inset_percent_y));
}
void Grid::draw_ellipse_filled()
{
	for(int i = first_index(); i <= last_index(); i++)
		::draw_ellipse_filled((*this)(i).left_top() + Point(2,2),
		               (*this)(i).right_bottom() - Point(2,2));
}
void Grid::draw_ellipse_filled(int inset_percent)
{
	draw_ellipse_filled(inset_percent, inset_percent);
}
void Grid::draw_ellipse_filled(int inset_percent_x, int inset_percent_y)
{
	for(int i = first_index(); i <= last_index(); i++)
		::draw_ellipse_filled(
		    (*this)(i).inset_left_top(inset_percent_x, inset_percent_y),
		    (*this)(i).inset_right_bottom(inset_percent_x, inset_percent_y));
}
void Grid::draw_text(std::string str)
{
	for(int i = first_index(); i <= last_index(); i++)
		::draw_text((*this)(i).center(), str);
}
void Grid::draw_text(char ch)
{
	for(int i = first_index(); i <= last_index(); i++)
		::draw_text((*this)(i).center(), ch);
}
void Grid::draw_text(int n)
{
	for(int i = first_index(); i <= last_index(); i++)
		::draw_text((*this)(i).center(), n);
}
void Grid::draw_text(unsigned int n)
{
	for(int i = first_index(); i <= last_index(); i++)
		::draw_text((*this)(i).center(), n);
}
void Grid::draw_text(double n)
{
	for(int i = first_index(); i <= last_index(); i++)
		::draw_text((*this)(i).center(), n);
}
std::string Grid::draw_input_text(std::string prompt)
{
	return ::draw_input_text(center(), prompt);
}

//Grid Mouse Input

bool Grid::is_click(int inset_percent)
{
	return is_click(inset_percent, inset_percent);
}
bool Grid::is_click(int inset_percent_x, int inset_percent_y)
{
	if(!::is_click())
		return false;
	Point p = ::get_click(false, false, false);
	return has_point(p, inset_percent_x, inset_percent_y);
}
GridCell Grid::get_click(int inset_percent)
{
	return get_click(inset_percent, inset_percent);
}
GridCell Grid::get_click(int inset_percent_x, int inset_percent_y)
{
	Point p;
	while(true)
	{
		p = ::get_click();
		if( has_point(p, inset_percent_x, inset_percent_y) )
			return point_to_cell(p);
	}
}
void Grid::remove_click(int inset_percent)
{
	if(this -> is_click(inset_percent))
		::remove_click();
}
void Grid::remove_click(int inset_percent_x, int inset_percent_y)
{
	if(this -> is_click(inset_percent_x, inset_percent_y))
		::remove_click();
}

//Grid Cell Reference Properties

//Accessors of Cell Reference

int Grid::first_row()
{
	return m_first_row;
}
int Grid::first_col()
{
	return m_first_col;
}
int Grid::first_index()
{
	return m_first_index;
}
int Grid::last_row()
{
	return m_first_row + m_num_rows - 1;
}
int Grid::last_col()
{
	return m_first_col + m_num_cols - 1;
}
int Grid::last_index()
{
	return m_first_index + num_cells() - 1;
}

//Mutators of Cell Reference

void Grid::set_first_row(int row)
{
	m_first_row = row;
}
void Grid::set_first_col(int col)
{
	m_first_col = col;
}
void Grid::set_first_index(int index)
{
	m_first_index = index;
}
void Grid::set_first(int first)
{
	m_first_row = m_first_col = m_first_index = first;
}

//Utilities of Cell Reference

int Grid::rowcol_to_index(int row, int col)
{
	return m_first_index + m_num_cols * (row - m_first_row) + (col - m_first_col);
}
int Grid::index_to_row(int index)
{
	return m_first_row + (index - m_first_index) / m_num_cols;
}
int Grid::index_to_col(int index)
{
	return m_first_col + (index - m_first_index) % m_num_cols;
}

//Grid Dimension Properties

//Accessors of Dimension

int Grid::num_rows()
{
	return m_num_rows;
}
int Grid::num_cols()
{
	return m_num_cols;
}
int Grid::num_cells()
{
	return m_num_rows * m_num_cols;
}

//Mutators of Dimension

void Grid::set_num_rows(int num_rows)
{
	if(num_rows > 0)
		m_num_rows = num_rows;
}
void Grid::set_num_cols(int num_cols)
{
	if(num_cols > 0)
		m_num_cols = num_cols;
}
void Grid::set_dimensions(int num_rows, int num_cols)
{
	set_num_rows(num_rows);
	set_num_cols(num_cols);
}

//Grid Position Properties

//Accessors of Position

Point Grid::center()
{
	return Point(x(), y());
}
Point Grid::midpoint()
{
	return Point(x(), y());
}
int Grid::x()
{
	return (m_left_top.x + m_right_bottom.x) / 2;
}
int Grid::y()
{
	return (m_left_top.y + m_right_bottom.y) / 2;
}
int Grid::left()
{
	return m_left_top.x;
}
int Grid::right()
{
	return m_right_bottom.x;
}
int Grid::top()
{
	return m_left_top.y;
}
int Grid::bottom()
{
	return m_right_bottom.y;
}
Point Grid::left_top()
{
	return m_left_top;
}
Point Grid::right_bottom()
{
	return m_right_bottom;
}
Point Grid::left_bottom()
{
	return Point(left(), bottom());
}
Point Grid::right_top()
{
	return Point(right(), top());
}

//Mutators of Position

void Grid::set_center(int x_center, int y_center)
{
	int oldx = x(), oldy = y();
	m_left_top.x = x_center - (oldx - m_left_top.x);
	m_left_top.y = y_center - (oldy - m_left_top.y);
	m_right_bottom.x = x_center + (m_right_bottom.x - oldx);
	m_right_bottom.y = y_center + (m_right_bottom.y - oldy);
}
void Grid::set_center(Point center)
{
	set_center(center.x, center.y);
}
void Grid::set_position(int x, int y, int rx, int ry)
{
	set_position(Point(x,y), rx, ry);
}
void Grid::set_position(Point center, int rx, int ry)
{
	m_left_top = Point(center.x - rx, center.y - ry);
	m_right_bottom = Point(center.x + rx, center.y + ry);
}
void Grid::set_position(Point left_top_pt, Point right_bottom_pt)
{
	//We cannot assume that left_top is really the left top...
	m_left_top = Point(min(left_top_pt.x, right_bottom_pt.x),
		               min(left_top_pt.y, right_bottom_pt.y));
	m_right_bottom = Point(max(left_top_pt.x, right_bottom_pt.x),
		               max(left_top_pt.y, right_bottom_pt.y));
}
void Grid::set_position(Grid grid)
{
	m_left_top = grid.m_left_top;
	m_right_bottom = grid.m_right_bottom;
}

//Utilities of Position

bool Grid::has_point(int x, int y, int inset_percent)
{
	return has_point(x, y, inset_percent, inset_percent);
}
bool Grid::has_point(int x, int y, int inset_percent_x, int inset_percent_y)
{
	Point new_left_top = inset_left_top(inset_percent_x, inset_percent_y);
	Point new_right_bottom = inset_right_bottom(inset_percent_x, inset_percent_y);
	return (new_left_top.x <= x) && (x <= new_right_bottom.x) && 
		   (new_left_top.y <= y) && (y <= new_right_bottom.y);
}
bool Grid::has_point(Point p, int inset_percent)
{
	return has_point(p.x, p.y, inset_percent);
}
bool Grid::has_point(Point p, int inset_percent_x, int inset_percent_y)
{
	return has_point(p.x, p.y, inset_percent_x, inset_percent_y);
}
GridCell Grid::point_to_cell(int x, int y)
{
	if(x < left()) x = left();
	else if(x >= right()) x = right() - 1;
	if(y < top()) y = top();
	else if(y >= bottom()) y = bottom() - 1;

	return (*this)( m_first_row + (y - m_left_top.y) * num_rows() / height(), 
		            m_first_col + (x - m_left_top.x) * num_cols() / width());
}
GridCell Grid::point_to_cell(Point p)
{
	return point_to_cell(p.x, p.y);
}

//Grid Size Functions

int Grid::cell_width(int inset_percent)
{
	return (100 - inset_percent) * (m_right_bottom.x - m_left_top.x) / (100*m_num_cols);
}
int Grid:: cell_width(int inset_percent_x, int inset_percent_y)
{
	return (100 - inset_percent_x) * (m_right_bottom.x - m_left_top.x) / (100*m_num_cols);
}
int Grid::cell_height(int inset_percent)
{
	return (100 - inset_percent) * (m_right_bottom.y - m_left_top.y) / (100*m_num_rows);
}
int Grid::cell_height(int inset_percent_x, int inset_percent_y)
{
	return (100 - inset_percent_y) * (m_right_bottom.y - m_left_top.y) / (100*m_num_rows);
}
int Grid::cell_radius_x(int inset_percent)
{
	return (100 - inset_percent) * cell_width() / 200;
}
int Grid::cell_radius_x(int inset_percent_x, int inset_percent_y)
{
	return (100 - inset_percent_x) * cell_width() / 200;
}
int Grid::cell_radius_y(int inset_percent)
{
	return (100 - inset_percent) * cell_height() / 200;
}
int Grid::cell_radius_y(int inset_percent_x, int inset_percent_y)
{
	return (100 - inset_percent_y) * cell_height() / 200;
}
int Grid::width(int inset_percent)
{
	return (100 - inset_percent) * (m_right_bottom.x - m_left_top.x) / 100;
}
int Grid::width(int inset_percent_x, int inset_percent_y)
{
	return (100 - inset_percent_x) * (m_right_bottom.x - m_left_top.x) / 100;
}
int Grid::height(int inset_percent)
{
	return (100 - inset_percent) * (m_right_bottom.y - m_left_top.y) / 100;
}
int Grid::height(int inset_percent_x, int inset_percent_y)
{
	return (100 - inset_percent_y) * (m_right_bottom.y - m_left_top.y) / 100;
}
int Grid::radius_x(int inset_percent)
{
	return (100 - inset_percent) * (m_right_bottom.x - m_left_top.x) / 200;
}
int Grid::radius_x(int inset_percent_x, int inset_percent_y)
{
	return (100 - inset_percent_x) * (m_right_bottom.x - m_left_top.x) / 200;
}
int Grid::radius_y(int inset_percent)
{
	return (100 - inset_percent) * (m_right_bottom.y - m_left_top.y) / 200;
}
int Grid::radius_y(int inset_percent_x, int inset_percent_y)
{
	return (100 - inset_percent_y) * (m_right_bottom.y - m_left_top.y) / 200;
}

//Subgrids

Grid Grid::subgrid(int first_row, int first_col, int last_row, int last_col)
{
	return subgrid((*this)(first_row, first_col), (*this)(last_row, last_col));
}
Grid Grid::subgrid(int first_index, int last_index)
{
	return subgrid((*this)(first_index), (*this)(last_index));
}
Grid Grid::subgrid(GridCell left_top_cell, GridCell right_bottom_cell)
{
	if(left_top_cell.col > right_bottom_cell.col)
	{
		int save = left_top_cell.col;
		left_top_cell = (*this)(left_top_cell.row, right_bottom_cell.col);
		right_bottom_cell = (*this)(right_bottom_cell.row, save);
	}
	if(left_top_cell.row > right_bottom_cell.row)
	{
		int save = left_top_cell.row;
		left_top_cell = (*this)(right_bottom_cell.row, left_top_cell.col);
		right_bottom_cell = (*this)(save, right_bottom_cell.col);
	}

	//Dimension properties
	Grid g(right_bottom_cell.row - left_top_cell.row + 1,
		   right_bottom_cell.col - left_top_cell.col + 1);

	//Cell Reference properties
	g.m_first_row   = left_top_cell.row;
	g.m_first_col   = left_top_cell.col;
	g.m_first_index = left_top_cell.index;

	//Position properties
	g.m_left_top = left_top_cell.m_left_top;
	g.m_right_bottom = right_bottom_cell.m_right_bottom;

	return g;
}
Grid Grid::grid_row(int which_row)
{
	return subgrid((*this)(which_row, first_col()), (*this)(which_row, last_col()));
}
Grid Grid::grid_col(int which_col)
{
	return subgrid((*this)(first_row(), which_col), (*this)(last_row(), which_col));
}

//General Grid Functions

void Grid::erase()	//erases interior inset by 1 pixel from boundary
{
	for(int i = first_index(); i <= last_index(); i++)
		::erase((*this)(i).m_left_top + Point(1,1), 
		        (*this)(i).m_right_bottom - Point(1,1));
}
void Grid::erase(Color color)
{
	Color c = get_background_color();
	set_background_color(color);
	erase();
	set_background_color(c);
}
void Grid::erase(int inset_percent)
{
	erase(inset_percent, inset_percent);
}
void Grid::erase(int inset_percent_x, int inset_percent_y)
{
	for(int i = first_index(); i <= last_index(); i++)
		::erase((*this)(i).inset_left_top(    inset_percent_x, inset_percent_y)
		                   + Point(1,1), 
		        (*this)(i).inset_right_bottom(inset_percent_x, inset_percent_y)
				           - Point(1,1));
}
Point Grid::random_point(int inset_percent)
{
	return random_point(inset_percent, inset_percent);
}
Point Grid::random_point(int inset_percent_x, int inset_percent_y)
{
	return ::random_point(inset_left_top(    inset_percent_x, inset_percent_y),
		                  inset_right_bottom(inset_percent_x, inset_percent_y));
}

//Grid Cursor Rescale Functions

int Grid::cursor_x_rescaled(int inset_percent)
{
	return cursor_x_rescaled(inset_percent, inset_percent);
}
int Grid::cursor_x_rescaled(int inset_percent_x, int inset_percent_y)
{
	return ::cursor_x_rescaled(inset_left_top(inset_percent_x, inset_percent_y).x,
		       inset_right_bottom(inset_percent_x, inset_percent_y).x);
}
int Grid::cursor_y_rescaled(int inset_percent)
{
	return cursor_y_rescaled(inset_percent, inset_percent);
}
int Grid::cursor_y_rescaled(int inset_percent_x, int inset_percent_y)
{
	return ::cursor_y_rescaled(inset_left_top(inset_percent_x, inset_percent_y).y,
		       inset_right_bottom(inset_percent_x, inset_percent_y).y);
}
Point Grid::cursor_point_rescaled(int inset_percent)
{
	return cursor_point_rescaled(inset_percent, inset_percent);
}
Point Grid::cursor_point_rescaled(int inset_percent_x, int inset_percent_y)
{
	return ::cursor_point_rescaled(inset_left_top(inset_percent_x, inset_percent_y),
		       inset_right_bottom(inset_percent_x, inset_percent_y));
}

//Grid Internal Helper functions
Point Grid::inset_left_top(int percent_x, int percent_y)
{
	return Point(
		m_left_top.x + (m_right_bottom.x - m_left_top.x) * percent_x / 200,
		m_left_top.y + (m_right_bottom.y - m_left_top.y) * percent_y / 200);

}
Point Grid::inset_right_bottom(int percent_x, int percent_y)
{
	return Point(
		m_right_bottom.x - (m_right_bottom.x - m_left_top.x) * percent_x / 200,
		m_right_bottom.y - (m_right_bottom.y - m_left_top.y) * percent_y / 200);
}
//*****************************************************************************
//*************************THE GridCell CLASS**********************************
//*****************************************************************************

GridCell::GridCell()
{
	Grid defaultGrid;
	(*this) = defaultGrid(defaultGrid.first_index());
}
GridCell::GridCell(Point left_top_pt, Point right_bottom_pt, 
				   int row_, int col_, int index_)
:Grid(1,1), row(row_), col(col_), index(index_)
{
	m_first_row = row;
	m_first_col = col;
	m_first_index = index;
	set_position(left_top_pt, right_bottom_pt);
}
//*****************************************************************************
//*************************THE SquareGrid CLASS********************************
//*****************************************************************************
SquareGrid::SquareGrid()
{
	(*this) = SquareGrid(2, 2);
}
SquareGrid::SquareGrid(int num_rows, int num_cols)
:Grid(num_rows > 0 ? num_rows : 2, num_cols > 0 ? num_cols : 2)
{
	set_position(Point(300,300), 300, 300);
}
SquareGrid::SquareGrid(int x, int y, int rx, int ry, int num_rows, int num_cols)
:Grid(num_rows > 0 ? num_rows : 2, num_cols > 0 ? num_cols : 2)
{
	set_position(x, y, rx, ry);
}
SquareGrid::SquareGrid(Point center, int rx, int ry, int num_rows, int num_cols)
:Grid(num_rows > 0 ? num_rows : 2, num_cols > 0 ? num_cols : 2)
{
	set_position(center, rx, ry);
}
SquareGrid::SquareGrid(Point left_top, Point right_bottom, int num_rows, int num_cols)
:Grid(num_rows > 0 ? num_rows : 2, num_cols > 0 ? num_cols : 2)
{
	set_position(left_top, right_bottom);
}
SquareGrid::SquareGrid(Grid subgrid, int num_rows, int num_cols)
:Grid(num_rows > 0 ? num_rows : 2, num_cols > 0 ? num_cols : 2)
{
	set_position(subgrid);
}
SquareGrid::SquareGrid(GridCell cell)
{
	(*this) = SquareGrid(cell, 1, 1);
}
SquareGrid::SquareGrid(int x, int y, int rx, int num_rows, int num_cols)
:Grid(num_rows > 0 ? num_rows : 2, num_cols > 0 ? num_cols : 2)
{
	set_position(x, y, rx);
}
SquareGrid::SquareGrid(Point center, int rx, int num_rows, int num_cols)
:Grid(num_rows > 0 ? num_rows : 2, num_cols > 0 ? num_cols : 2)
{
	set_position(center, rx);
}

//SquareGrid Dimension Mutators

void SquareGrid::set_num_rows(int num_rows)
{
	if(num_rows <= 0)
		return;
	m_num_rows = num_rows;
	set_position( center(), radius_x(), radius_y());
}
void SquareGrid::set_num_cols(int num_cols)
{
	if(num_cols <= 0)
		return;
	m_num_cols = num_cols;
	set_position( center(), radius_x(), radius_y());
}
void SquareGrid::set_dimensions(int num_rows, int num_cols)
{
	if(num_rows > 0)
		m_num_rows = num_rows;
	if(num_cols > 0)
		m_num_cols = num_cols;
	set_position( center(), radius_x(), radius_y() );
}

//SquareGrid Position Mutators

void SquareGrid::set_position(int x, int y, int rx, int ry)
{
	set_position(Point(x,y), rx, ry);
}
void SquareGrid::set_position(Point center, int rx, int ry)
{
	if(rx < 0) rx = -rx;
	if(ry < 0) ry = -ry;
	if(calculate_ry(rx) > ry)
		rx = ry * rx / calculate_ry(rx);
	m_left_top = center - Point(rx, calculate_ry(rx));
	m_right_bottom = center + Point(rx, calculate_ry(rx));
}
void SquareGrid::set_position(Point left_top, Point right_bottom)
{
	//We cannot assume that left_top is really the left top...
	Point ltp = Point(min(left_top.x, right_bottom.x),
		              min(left_top.y, right_bottom.y));
	Point rbp = Point(max(left_top.x, right_bottom.x),
		              max(left_top.y, right_bottom.y));

	Point center = Point((ltp.x + rbp.x)/2, 
						 (ltp.y + rbp.y)/2);
	int rx = rbp.x - center.x;
	int ry = rbp.y - center.y;
	set_position(center, rx, ry);	
}
void SquareGrid::set_position(Grid subgrid)
{
	set_position(subgrid.center(), subgrid.radius_x(), subgrid.radius_y());
}
void SquareGrid::set_position(int x, int y, int rx)
{
	set_position(Point(x,y), rx);
}
void SquareGrid::set_position(Point center, int rx)
{
	if(rx < 0) rx = -rx;
	m_left_top = center - Point(rx, calculate_ry(rx));
	m_right_bottom = center + Point(rx, calculate_ry(rx));
}

//SquareGrid Size Function

int SquareGrid::cell_radius(int inset_percent)
{
	return cell_radius_x(inset_percent);
}

//SquareGrid Internal Helper Function
int SquareGrid::calculate_ry(int rx)
{
	return rx * num_rows() / num_cols();
}

//*****************************************************************************
//*************************THE Ring CLASS**************************************
//*****************************************************************************

//Ring Constructors

Ring::Ring()
{
	(*this) = Ring(6);
}
Ring::Ring(int num_cells)
:m_center(Point(100,100)), m_radius(100), m_num_cells(num_cells), 
 m_angle(0), m_first_index(0)
{

}
Ring::Ring(int x, int y, int r, int num_cells, int ang)
:m_center(Point(x,y)), m_radius(r), m_num_cells(num_cells), 
 m_angle(ang), m_first_index(0)
{

}
Ring::Ring(Point center, int r, int num_cells, int ang)
:m_center(center), m_radius(r), m_num_cells(num_cells),
 m_angle(ang), m_first_index(0)
{

}

//Obtaining RingCells

RingCell Ring::operator()(int index)
{
	if(m_num_cells == 1)
		return RingCell(m_center, m_radius, index);
	return RingCell(circle_point(m_center, m_radius, 
		(index - first_index()) * 360 / m_num_cells + m_angle),
		cell_radius(), index);
}
RingCell Ring::cursor_cell()
{
	return point_to_cell(CurPt);
}
RingCell Ring::random_cell()
{
	return (*this)(random(first_index(), last_index()));
}

//Ring Drawing Functions

void Ring::draw(int inset_percent)
{
	for(int i = first_index(); i <= last_index(); i++)
		::draw_circle((*this)(i).center(), cell_radius(inset_percent));
}
void Ring::draw_filled(int inset_percent)
{
	for(int i = first_index(); i <= last_index(); i++)
		::draw_circle_filled((*this)(i).center(), cell_radius(inset_percent));
}
void Ring::draw_circle(int inset_percent)
{
	draw(inset_percent);
}
void Ring::draw_circle_filled(int inset_percent)
{
	draw_filled(inset_percent);
}
void Ring::draw_text(std::string str)
{
	::draw_text(center(), str);
}
void Ring::draw_text(char ch)
{
	::draw_text(center(), ch);
}
void Ring::draw_text(int n)
{
	::draw_text(center(), n);
}
void Ring::draw_text(unsigned int n)
{
	::draw_text(center(), n);
}
void Ring::draw_text(double n)
{
	::draw_text(center(), n);
}
std::string Ring::draw_input_text(std::string prompt)
{
	return ::draw_input_text(center(), prompt);
}

//Ring Mouse Input

bool Ring::is_click(int inset_percent)
{
	if(!::is_click())
		return false;
	Point p = ::get_click(false, false, false);
	return has_point(p, inset_percent);
}
RingCell Ring::get_click(int inset_percent)
{
	Point p;
	while(true)
	{
		p = ::get_click();
		for(int i = first_index(); i <= last_index(); i++)
			if(distance((*this)(i).center(), p) < cell_radius(inset_percent))
				return (*this)(i);
	}
}
void Ring::remove_click(int inset_percent)
{
	if(this -> is_click(inset_percent))
		::remove_click();
}

//Ring Cell Reference Properties

//Accessors of Cell Reference

int Ring::first_index()
{
	return m_first_index;
}
int Ring::last_index()
{
	return m_first_index + m_num_cells - 1;
}

//Mutators of Cell Reference

void Ring::set_first_index(int index)
{
	m_first_index = index;
}
void Ring::set_first(int index)
{
	m_first_index = index;
}

//Ring Dimension Properties

//Accessora of Dimension
int Ring::num_cells()
{
	return m_num_cells;
}
//Mutators of Dimension
void Ring::set_num_cells(int num_cells)
{
	if(num_cells > 0)
		m_num_cells = num_cells;
}

//Ring Position Properties

//Accessors of Position

Point Ring::center()
{
	return m_center;
}
Point Ring::midpoint()
{
	return m_center;
}
Ring::operator Point()
{
	return m_center;
}
int Ring::x()
{
	return m_center.x;
}
int Ring::y()
{
	return m_center.y;
}
int Ring::angle()
{
	return m_angle;
}

//Mutators of Position

void Ring::set_center(int x, int y)
{
	m_center = Point(x,y);
}
void Ring::set_center(Point center)
{
	m_center = center;
}
void Ring::set_position(int x, int y, int radius)
{
	m_center.x = x;
	m_center.y = y;
	m_radius = radius;
}
void Ring::set_position(int x, int y, int r, int ang)
{
	m_center.x = x;
	m_center.y = y;
	m_radius = r;
	m_angle = ang;
}
void Ring::set_position(Point center, int r)
{
	set_position(center.x, center.y, r);
}
void Ring::set_position(Point center, int r, int ang)
{
	set_position(center.x, center.y, r, ang);
}
void Ring::set_angle(int angle)
{
	m_angle = angle;
}

//Utilities of Position

bool Ring::has_point(int x, int y, int inset_percent)
{
	for(int i = first_index(); i <= last_index(); i++)
		if(distance((*this)(i).center(), Point(x,y)) < cell_radius(inset_percent))
			return true;
	return false;
}
bool Ring::has_point(Point p, int inset_percent)
{
	return has_point(p.x, p.y, inset_percent);
}
RingCell Ring::point_to_cell(int x, int y)
{
	RingCell close_cell = (*this)(first_index());
	double min_dist = distance(close_cell.center(), x, y);
	for(int i = first_index() + 1; i <= last_index(); i++)
		if(distance((*this)(i).center(), Point(x,y)) < min_dist)
		{
			close_cell = (*this)(i);
			min_dist = distance((*this)(i).center(), Point(x,y));
		}
	return close_cell;
}
RingCell Ring::point_to_cell(Point p)
{
	return point_to_cell(p.x, p.y);
}
int Ring::index_to_angle(int index)
{
	return (index - first_index()) * 360 / m_num_cells + m_angle;
}

//Ring Size Functions

int Ring::cell_radius(int inset_percent)
{
	if(m_num_cells == 1)
		return (100 - inset_percent) * m_radius / 100;
	return int(distance( circle_point(m_center, m_radius, 0),
		              circle_point(m_center, m_radius, 360 / m_num_cells)) *
					  (100 - inset_percent)/ 200.0);
}
int Ring::radius(int inset_percent)
{
	return (100 - inset_percent) * m_radius / 100;
}

//General Ring Functions

void Ring::erase()	//erases interior inset by 1 pixel from boundary
{
	for(int i = first_index(); i <= last_index(); i++)
		::draw_circle_filled(center(), cell_radius() - 1);
}
void Ring::erase(Color color)
{
	Color c = get_fill_color();
	set_fill_color(color);
	erase();
	set_fill_color(c);
}
void Ring::erase(int inset_percent)
{
	for(int i = first_index(); i <= last_index(); i++)
		::draw_circle_filled(center(), (100 - inset_percent) * cell_radius() / 100);
}
Point Ring::random_point(int inset_percent)
{
	RingCell cell = random_cell();
	int d = random(0, cell_radius());
	int a = random(0,360);
	return circle_point(cell.center(), d, a);
}
int Ring::index_to_row(int index)
{
	return 0;
}
int Ring::index_to_col(int index)
{
	return index;
}
int Ring::rowcol_to_index(int row, int col)
{
	return col;
}
//*****************************************************************************
//*************************THE RingCell CLASS**********************************
//*****************************************************************************

RingCell::RingCell()
:Ring(1), index(m_first_index)
{
	m_radius = 0;
	m_angle = 0;
}
RingCell::RingCell(Point center, int radius, int index_)
:Ring(center, radius, 1), index(index_)
{
	m_first_index = index;
	m_angle = 0;
}
//*****************************************************************************
//****************************THE Hex CLASS************************************
//*****************************************************************************

//Hex Constructors

Hex::Hex()
{
	(*this) = Hex(4,2);
}
Hex::Hex(int num_rows, int num_cols)
{
	(*this) = Hex(Point(50,50), 50, 50, num_rows, num_cols);
}
Hex::Hex(int x, int y, int rx, int ry, int num_rows, int num_cols)
{
	(*this) = Hex(Point(x,y), rx, ry, num_rows, num_cols);
}
Hex::Hex(Point center, int rx, int ry, int num_rows, int num_cols)

	//Dimension properties
   :m_num_rows(num_rows > 0 ? num_rows : 2),
	m_num_cols(num_cols > 0 ? num_cols : 2),
	
	 //Cell reference properties
	m_first_index(0),
	m_first_row(0),
	m_first_col(0),

	//Position Properties
	m_center(center)
{
	if(rx < 0) rx = -rx;
	if(ry < 0) ry = -ry;

	if(calculate_ry(rx) > ry)
		rx = rx * ry / calculate_ry(rx);
	m_rx = rx;
}
Hex::Hex(Point left_top, Point right_bottom, int num_rows, int num_cols)
{
	//We cannot assume that left_top is really the left top...
	Point ltp = Point(min(left_top.x, right_bottom.x),
		              min(left_top.y, right_bottom.y));
	Point rbp = Point(max(left_top.x, right_bottom.x),
		              max(left_top.y, right_bottom.y));

	Point center = (ltp + rbp) / 2;
	int rx = rbp.x - center.x;
	int ry = rbp.y - center.y;
	(*this) = Hex(center, rx, ry, num_rows, num_cols);
}
Hex::Hex(int x, int y, int rx, int num_rows, int num_cols)
{
	(*this) = Hex(Point(x,y), rx, num_rows, num_cols);
}
Hex::Hex(Point center, int rx, int num_rows, int num_cols)

	//Dimension properties
   :m_num_rows(num_rows > 0 ? num_rows : 2),
	m_num_cols(num_cols > 0 ? num_cols : 2),
	
	 //Cell reference properties
	m_first_index(0),
	m_first_row(0),
	m_first_col(0),

	//Position Properties
	m_center(center),
	m_rx(rx < 0 ? -rx : rx)
{

}

Grid Hex::grid()
{
	Grid g(m_num_rows, m_num_cols);

	//These next three lines are essential!
	g.set_first_row(m_first_row);
	g.set_first_col(m_first_col);
	g.set_first_index(m_first_index);

	g.set_position(m_center, m_rx, calculate_ry(m_rx));
	return g;
}

//Obtaining HexCells

HexCell Hex::operator()(int row, int col)
{
	GridCell cell = grid()(row, col);
	return HexCell(cell.center(), cell.radius_x(), 
		           row, col, rowcol_to_index(row, col));
}
HexCell Hex::operator()(int index)
{
	return (*this)(index_to_row(index), index_to_col(index));
}
HexCell Hex::cursor_cell()
{
	return point_to_cell(CurPt);
}
HexCell Hex::random_cell()
{
	return (*this)(random(first_index(), last_index()));
}

//Hex Drawing Functions
void Hex::draw(bool draw_grid, bool hexlines)
{
	if(draw_grid)
		grid().draw();
	if(!hexlines)
		return;

	for(int i = first_index(); i <= last_index(); i++)
		::draw_ngon((*this)(i).center(), cell_out_radius(), 6);
}
void Hex::draw(int inset_percent)
{
	for(int i = first_index(); i <= last_index(); i++)
		::draw_ngon((*this)(i).center(), 
		    (100 - inset_percent) * cell_out_radius() / 100, 6);
}
void Hex::draw_circle(int inset_percent)
{
	for(int i = first_index(); i <= last_index(); i++)
		::draw_circle((*this)(i).center(), 
		    (100 - inset_percent) * cell_in_radius() / 100);
}
void Hex::draw_circle_filled(int inset_percent)
{
	for(int i = first_index(); i <= last_index(); i++)
		::draw_circle_filled((*this)(i).center(), 
		    (100 - inset_percent) * cell_in_radius() / 100);
}
void Hex::draw_filled(int inset_percent)
{
	for(int i = first_index(); i <= last_index(); i++)
		::draw_ngon_filled((*this)(i).center(), 
		    (100 - inset_percent) * cell_out_radius() / 100, 6);
}
void Hex::draw_text(std::string str)
{
	for(int i = first_index(); i <= last_index(); i++)
		::draw_text((*this)(i).center(), str);
}
void Hex::draw_text(char ch)
{
	for(int i = first_index(); i <= last_index(); i++)
		::draw_text((*this)(i).center(), ch);
}
void Hex::draw_text(int n)
{
	for(int i = first_index(); i <= last_index(); i++)
		::draw_text((*this)(i).center(), n);
}
void Hex::draw_text(unsigned int n)
{
	for(int i = first_index(); i <= last_index(); i++)
		::draw_text((*this)(i).center(), n);
}
void Hex::draw_text(double n)
{
	for(int i = first_index(); i <= last_index(); i++)
		::draw_text((*this)(i).center(), n);
}
std::string Hex::draw_input_text(std::string prompt)
{
	return ::draw_input_text(center(), prompt);
}

//Hex Mouse Input

bool Hex::is_click(int inset_percent)
{
	if(!::is_click())
		return false;
	Point p = ::get_click(false, false, false);
	return has_point(p, inset_percent);
}
HexCell Hex::get_click(int inset_percent)
{
	Point p;
	while(true)
	{
		p = ::get_click();
		if( has_point(p, inset_percent) )
			return point_to_cell(p);
	}
}
void Hex::remove_click(int inset_percent)
{
	if(this -> is_click(inset_percent))
		::remove_click();
}

//Hex Cell Reference Properties

//Accessors of Cell Reference

int Hex::first_row()
{
	return m_first_row;
}
int Hex::first_row(int which_col)
{
	return first_row() + (which_col - first_col()) % 2;
}
int Hex::first_col()
{
	return m_first_col;
}
int Hex::first_index()
{
	return m_first_index;
}
int Hex::first_col(int which_row)
{
	return first_col() + (which_row - first_row()) % 2;
}
int Hex::last_row()
{
	return m_first_row + num_rows() - 1;
}
int Hex::last_row(int which_col)
{
	return last_row() - 
		((last_row() + which_col) + (first_row() + first_col())) % 2;
}
int Hex::last_col()
{
	return m_first_col + num_cols() - 1;
}
int Hex::last_col(int which_row)
{
	return last_col() - 
		((last_col() + which_row) - (first_row() + first_col())) % 2;
}
int Hex::last_index()
{
	return m_first_index + num_cells() - 1;
}

//Mutators of Cell Reference

void Hex::set_first_row(int row)
{
	m_first_row = row;
}
void Hex::set_first_col(int col)
{
	m_first_col = col;
}
void Hex::set_first_index(int index)
{
	m_first_index = index;
}
void Hex::set_first(int first)
{
	m_first_row = m_first_col = m_first_index = first;
}
//Utilities of Cell Reference

int Hex::rowcol_to_index(int row, int col)
{
	return ((row - first_row()) / 2) * num_cols() + (col - first_col()) / 2 +
		((row - first_row()) % 2 ?  (num_cols() + 1) / 2 : 0) + first_index();
}
int Hex::index_to_row(int index)
{
	return (index - first_index()) / num_cols() * 2 + 
		(2 * ((index - first_index()) % num_cols()) >= num_cols() ? 1 : 0) +
		first_row();
}
int Hex::index_to_col(int index)
{
	if(2 * ((index - first_index()) % num_cols()) < num_cols())
		return (2 * (index - first_index())) % num_cols() + first_col();
	return 2 * ((index - first_index()) % num_cols() - (num_cols() + 1) / 2) + 1
		+ first_col();
}

//Hex Dimension Properties

//Accessors of Dimension

int Hex::num_rows()
{
	return m_num_rows;
}
int Hex::num_cols()
{
	return m_num_cols;
}
int Hex::num_cells()
{
	return ((m_num_rows + 1) / 2) * ((m_num_cols + 1) / 2) +
		    ((m_num_rows / 2) * (m_num_cols / 2));
}

//Mutators of Dimension

void Hex::set_num_rows(int num_rows)
{
	if(num_rows < 1)
		return;
	m_num_rows = num_rows;
	set_position(center(), grid_radius_x());
}
void Hex::set_num_cols(int num_cols)
{
	if(num_cols < 1)
		return;
	m_num_cols = num_cols;
	set_position(center(), grid_radius_x());
}
void Hex::set_dimensions(int num_rows, int num_cols)
{
	if(num_rows > 0)
		m_num_rows = num_rows;
	if(num_cols > 0)
		m_num_cols = num_cols;
	set_position(center(), grid_radius_x());
}

//Hex Position Properties

//Accessors of Position

Point Hex::center()
{
	return Point(x(), y());
}
Point Hex::midpoint()
{
	return Point(x(), y());
}
int Hex::x()
{
	return m_center.x;
}
int Hex::y()
{
	return m_center.y;
}

//Mutators of Position

void Hex::set_center(int x, int y)
{
	m_center = Point(x,y);
}
void Hex::set_center(Point center)
{
	m_center = center;
}
void Hex::set_position(int x, int y, int rx, int ry)
{
	set_position(Point(x,y), rx, ry);
}
void Hex::set_position(Point center, int rx, int ry)
{
	if(rx < 0) rx = -rx;
	if(ry < 0) ry = -ry;

	if(calculate_ry(rx) > ry)
		rx = rx * ry / calculate_ry(rx);

	m_center = center;
	m_rx = rx;
}
void Hex::set_position(Point left_top, Point right_bottom)
{
	Point ltp = Point(min(left_top.x, right_bottom.x),
		              min(left_top.y, right_bottom.y));
	Point rbp = Point(max(left_top.x, right_bottom.x),
		              max(left_top.y, right_bottom.y));

	Point center = (ltp + rbp) / 2;
	int rx = rbp.x - center.x;
	int ry = rbp.y - center.y;
	set_position(center, rx, ry);
}
void Hex::set_position(int x, int y, int rx)
{
	set_position(Point(x,y),rx);
}
void Hex::set_position(Point center, int rx)
{
	if(rx < 0) rx = -rx;
	m_center = center;
	m_rx = rx;
}
//Utilities of Position

bool Hex::has_point(int x, int y, int inset_percent)
{
	if(grid().has_point(x,y))
		return true;
	for(int i = first_index(); i <= last_index(); i++)
		if(distance(x, y, (*this)(i).center()) <= 
		   (100 - inset_percent) * cell_in_radius() / 100)
			return true;
	return false;
}
bool Hex::has_point(Point p, int inset_percent)
{
	return has_point(p.x, p.y, inset_percent);
}
HexCell Hex::point_to_cell(int x, int y)
{
	for(int i = first_index(); i <= last_index(); i++)
		if((*this)(i).has_point(x,y))
			return (*this)(i);
	double min_dist = distance((*this)(first_index()).center(), x, y);
	HexCell close_cell = (*this)(first_index());
	for(int i = first_index() + 1; i <= last_index(); i++)
		if(distance(x,y,(*this)(i).center()) < min_dist)
		{
			min_dist = distance(x,y,(*this)(i).center());
			close_cell = (*this)(i);
		}
	return close_cell;
}
HexCell Hex::point_to_cell(Point p)
{
	return point_to_cell(p.x, p.y);
}

//Hex Size Functions

int Hex::cell_in_radius(int inset_percent)
{
	return (100 - inset_percent) * (grid().height() / num_rows()) / 100;
}
int Hex::cell_out_radius(int inset_percent)
{
	return int((100 - inset_percent) * 2.0 * grid().cell_width() / 300);
}
int Hex::grid_radius_x(int inset_percent)
{
	return (100 - inset_percent) * m_rx / 100;
}
int Hex::grid_radius_y(int inset_percent)
{
	return (100 - inset_percent) * calculate_ry(m_rx) / 100;
}

//Subhex

Hex Hex::subhex(int first_row, int first_col, int last_row, int last_col)
{
	return subhex((*this)(first_row, first_col),
		          (*this)(last_row , last_col ));
}
Hex Hex::subhex(int first_index, int last_index)
{
	return subhex((*this)(first_index), (*this)(last_index));
}
Hex Hex::subhex(HexCell left_top_cell, HexCell right_bottom_cell)
{
	if(left_top_cell.col > right_bottom_cell.col)
	{
		int save = left_top_cell.col;
		left_top_cell = (*this)(left_top_cell.row, right_bottom_cell.col);
		right_bottom_cell = (*this)(right_bottom_cell.row, save);
	}
	if(left_top_cell.row > right_bottom_cell.row)
	{
		int save = left_top_cell.row;
		left_top_cell = (*this)(right_bottom_cell.row, left_top_cell.col);
		right_bottom_cell = (*this)(save, right_bottom_cell.col);
	}

	//Dimension members
	Hex h(right_bottom_cell.row - left_top_cell.row + 1,
		  right_bottom_cell.col - left_top_cell.col + 1);

	h.set_first_row(left_top_cell.row);
	h.set_first_col(left_top_cell.col);
	h.set_first_index(left_top_cell.index);

	//Position members
	Point p = left_top_cell.grid().left_top();
	Point q = right_bottom_cell.grid().right_bottom();
	h.m_center = Point( (p.x + q.x) / 2, (p.y + q.y) / 2);
	h.m_rx = (q.x - p.x) / 2;

	return h;
}
Hex Hex::hex_row(int which_row)
{
	return subhex(which_row, first_col(which_row), which_row, last_col(which_row));
}
Hex Hex::hex_col(int which_col)
{
	return subhex(first_row(which_col), which_col, last_row(which_col), which_col);
}

//General Hex Functions

void Hex::erase()	//erases interior inset by 1 pixel from boundary
{
	for(int i = first_index(); i <= last_index(); i++)
		::draw_ngon_filled(center(), cell_out_radius() - 1, 6);
}
void Hex::erase(Color color)
{
	Color c = get_fill_color();
	set_fill_color(color);
	erase();
	set_fill_color(c);
}
void Hex::erase(int inset_percent)
{
	for(int i = first_index(); i <= last_index(); i++)
		::draw_ngon_filled(center(), (100 - inset_percent) * cell_out_radius() / 100, 6);
}
Point Hex::random_point(int inset_percent)
{
	return grid().random_point(inset_percent);
}

//Hex Internal Helper Function

int Hex::calculate_ry(int rx)
{
	return int(rx * num_rows() * .577350269 / num_cols());
}
//*****************************************************************************
//*************************THE HexCell CLASS***********************************
//*****************************************************************************
HexCell::HexCell()
:Hex(1,1),row(m_first_row),col(m_first_col),index(m_first_index)
{

}
HexCell::HexCell(Point center, int rx, int row_, int col_, int index_)
:Hex(1,1), row(row_), col(col_), index(index_)
{
	m_first_row = row;
	m_first_col = col;
	m_first_index = index;
	set_position(center, rx);
}


//HexCell Size Functions

int HexCell::out_radius(int inset_percent)
{
	return cell_out_radius(inset_percent);
}
int HexCell::in_radius(int inset_percent)
{
	return cell_in_radius(inset_percent);
}

//*****************************************************************************
//*************************THE Shape CLASSES***********************************
//*****************************************************************************
int Shape::max_sides = 60;	//static member of Shape

Shape::Shape(int x_, int y_, int rx_, int ry_, int n_, int rot_, int out_)
:x(x_), y(y_), rx(rx_), ry(ry_), num_sides(n_), rot(rot_), out(out_)
{

}
Shape::Shape(Point center, int rx_, int ry_, int n_, int rot_, int out_)
:x(center.x), y(center.y), rx(rx_), ry(ry_), num_sides(n_), rot(rot_), out(out_)
{

}
Shape::Shape(Point left_top, Point right_bottom, int n_, int rot_, int out_)
:x((left_top.x + right_bottom.x)/2), y((left_top.y+right_bottom.y)/2), 
 rx((right_bottom.x - left_top.x)/2), ry((right_bottom.y - left_top.y)/2),
 num_sides(n_), rot(rot_), out(out_)
{

}
void Shape::draw()
{
	int new_n = (num_sides >= 3 ? num_sides : Shape::max_sides);
	double scaler = (out == -100 ? 1.0 :
		200.0 / (200.0 + (100.0 + double(out)) * (cos(PI / new_n) - 1)));
	int new_rx = int(double(rx) * scaler);
	int new_ry = int(double(ry) * scaler);
	draw_elliptic_ngon(x, y, new_rx, new_ry, new_n, rot);
};
void Shape::draw_filled()
{
	int new_n = (num_sides >= 3 ? num_sides : Shape::max_sides);
	double scaler = (out == -100 ? 1.0 :
		200.0 / (200.0 + (100.0 + double(out)) * (cos(PI / new_n) - 1)));
	int new_rx = int(double(rx) * scaler);
	int new_ry = int(double(ry) * scaler);
	draw_elliptic_ngon_filled(x, y, new_rx, new_ry, new_n, rot);
};

void Shape::draw_3D()
{
	int new_n = (num_sides >= 3 ? num_sides : Shape::max_sides);
	double scaler = (out == -100 ? 1.0 :
		200.0 / (200.0 + (100.0 + double(out)) * (cos(PI / new_n) - 1)));
	int new_rx = int(double(rx) * scaler);
	int new_ry = int(double(ry) * scaler);
	draw_elliptic_ngon_3D(x, y, new_rx, new_ry, new_n, rot);
};
void Shape::draw_3D_filled()
{
	draw_3D();
	draw_filled();
};
void Shape::draw_P3D()
{
	int new_n = (num_sides >= 3 ? num_sides : Shape::max_sides);
	double scaler = (out == -100 ? 1.0 :
		200.0 / (200.0 + (100.0 + double(out)) * (cos(PI / new_n) - 1)));
	int new_rx = int(double(rx) * scaler);
	int new_ry = int(double(ry) * scaler);
	draw_elliptic_ngon_P3D(x, y, new_rx, new_ry, new_n, rot);
};
void Shape::draw_P3D_filled()
{
	draw_P3D();
	draw_filled();
	return;
};
Point Shape::point(int angle)
{
	int new_n = (num_sides >= 3 ? num_sides : Shape::max_sides);
	while(angle < 0)
		angle += 360;
	angle = angle - ( int(270 - 180.0 / new_n) % (360/new_n) );
	while(angle < 0)
		angle += 360;
	angle %= 360;
	Point* vertex_array = new Point[new_n + 1];
	vertices(vertex_array);
	vertex_array[new_n] = vertex_array[0];
	Point returnPt = range(vertex_array, new_n + 1, angle / 360.0);
	delete[] vertex_array;
	return returnPt;
};
Point Shape::point(Point ray_point)
{
	return point(int(circle_angle(x, y, ray_point)));
};
void Shape::vertices(Point* vertex_array)
{
	int new_n = (num_sides >= 3 ? num_sides : Shape::max_sides);
	double scaler = (out == -100 ? 1.0 :
		200.0 / (200.0 + (100.0 + double(out)) * (cos(PI / new_n) - 1)));
	int new_rx = int(double(rx) * scaler);
	int new_ry = int(double(ry) * scaler);

	vertices_elliptic_ngon(vertex_array, x, y, new_rx, new_ry, new_n, rot);
}

Circle::Circle(int x_, int y_, int r, int rot_):
Shape(x_, y_, r, r, 0, rot_, 0)
{

}
Circle::Circle(Point center, int r, int rot_):
Shape(center.x, center.y, r, r, 0, rot_, 0)
{

}
Ellipse::Ellipse(int x_, int y_, int rx_, int ry_, int rot_):
Shape(x_, y_, rx_, ry_, 0, rot_, 0)
{

}
Ellipse::Ellipse(Point center, int rx_, int ry_, int rot_):
Shape(center.x, center.y, rx_, ry_, 0, rot_, 0)
{

}
Ellipse::Ellipse(Point left_top, Point right_bottom, int rot_)
:Shape(left_top, right_bottom, 0, rot_, 0)
{

}
EllipticNgon::EllipticNgon(int x_, int y_, int rx_, int ry_, int n_, int rot_):
Shape(x_, y_, rx_, ry_, n_, rot_, -100)
{

}
EllipticNgon::EllipticNgon(Point center, int rx_, int ry_, int n_, int rot_):
Shape(center.x, center.y, rx_, ry_, n_, rot_, -100)
{

}
EllipticNgon::EllipticNgon(Point left_top, Point right_bottom, int n_, int rot_)
:Shape(left_top, right_bottom, n_, rot_, -100)
{

}
EllipticOutgon::EllipticOutgon(int x_, int y_, int rx_, int ry_, int n_, int rot_):
Shape(x_, y_, rx_, ry_, n_, rot_, 100)
{

}
EllipticOutgon::EllipticOutgon(Point center, int rx_, int ry_, int n_, int rot_):
Shape(center.x, center.y, rx_, ry_, n_, rot_, 100)
{

}
EllipticOutgon::EllipticOutgon(Point left_top, Point right_bottom, int n_, int rot_)
:Shape(left_top, right_bottom, n_, rot_, 100)
{

}
Ngon::Ngon(int x_, int y_, int r, int n_, int rot_):
Shape(x_, y_, r, r, n_, rot_, -100)
{

}
Ngon::Ngon(Point center, int r, int n_, int rot_):
Shape(center.x, center.y, r, r, n_, rot_, -100)
{

}
Outgon::Outgon(int x_, int y_, int r, int n_, int rot_):
Shape(x_, y_, r, r, n_, rot_, 100)
{

}
Outgon::Outgon(Point center, int r, int n_, int rot_):
Shape(center.x, center.y, r, r, n_, rot_, 100)
{

}
Rectangle::Rectangle(int x_, int y_, int rx_, int ry_, int rot_):
Shape(x_, y_, rx_, ry_, 4, rot_, 100)
{

}
Rectangle::Rectangle(Point center, int rx_, int ry_, int rot_):
Shape(center.x, center.y, rx_, ry_, 4, rot_, 100)
{

}
Rectangle::Rectangle(Point left_top, Point right_bottom, int rot_)
:Shape(left_top, right_bottom, 4, rot_, 100)
{

}
Square::Square(int x_, int y_, int r, int rot_):
Shape(x_, y_, r, r, 4, rot_, 100)
{

}
Square::Square(Point center, int r, int rot_):
Shape(center.x, center.y, r, r, 4, rot_, 100)
{

}
//*****************************************************************************
//*************************THE VariableShape CLASS*****************************
//*****************************************************************************
VariableShape::VariableShape(double seconds, Shape lim1, Shape lim2, bool is_fixed_rate)
{
	limit1 = lim1;
	limit2 = lim2;
	old_val = random_shape(limit1, limit2);
	new_val = random_shape(limit1, limit2);
	fixed_rate_mode = is_fixed_rate;
	if(seconds <= 0)
		seconds = .001;
	if(fixed_rate_mode)	//seconds specifies the time between limits, determining rate
	{
		rate = ::distance(limit1, limit2) / seconds;
		calc_period();
	}
	else	//seconds now gives the fixed period
	{
		period = seconds;
		//if not in fixed rate mode, then rate gotten from get_rate();
	}
	last_change_time = run_time();
}
VariableShape::VariableShape(double seconds, bool is_fixed_rate)
{
	*this = VariableShape(seconds, Shape(PerX(25),PerY(25),PerX(5),PerY(5),3,0,-100), Shape(PerX(75),PerY(75),PerX(35),PerY(35),8,360,100),is_fixed_rate);
}

//Conversion operator allows VariableShapes to be used as Shape.
VariableShape::operator Shape()
{
	if(run_time() - last_change_time > period * 1000)
	{
		old_val = new_val;
		new_val = random_shape(limit1, limit2);
		last_change_time = run_time();
	}
	if(fixed_rate_mode)
		calc_period();
	if(period <= 0)
		period = .001;
	return range(old_val, new_val, 
		 (run_time() - last_change_time) / (1000.0 * period));
}

void VariableShape::begin()
{
	last_change_time = run_time();
}
void VariableShape:: end()
{
	old_val = new_val;
	new_val = random_shape(limit1, limit2);
	last_change_time = run_time();
	if(fixed_rate_mode)
		calc_period();
}
void VariableShape::reset()
{
	old_val = random_shape(limit1, limit2);
	new_val = random_shape(limit1, limit2);
	last_change_time = run_time();
	if(fixed_rate_mode)
		calc_period();
}
void VariableShape::reset(Shape start)
{
	old_val = start;
	new_val = random_shape(limit1, limit2);
	last_change_time = run_time();
	if(fixed_rate_mode)
		calc_period();
}
void VariableShape::reverse()
{
	Shape temp = new_val;
	new_val = old_val;
	old_val = temp;
	last_change_time = int(2 * run_time() - last_change_time - 1000 * period);
}

//Change the limits of possible values
//Note that we will still finish the present transition
void VariableShape::set_limits(Shape lim1, Shape lim2, bool reset_it)
{
	limit1 = lim1;
	limit2 = lim2;
	if(fixed_rate_mode)
		calc_period();
	if(reset_it)
		reset();
}

//How long between changes?
double VariableShape::get_period()
{
	return period;
}
void VariableShape::set_period(double seconds)
{
	fixed_rate_mode = false;
	period = seconds;
}
double VariableShape::get_rate()
{
	if(fixed_rate_mode)
		return rate;
	else
	{
		double d = distance();
		if(d == 0 || period == 0)
			return 0;
		else
			return d / period;
	}
}
void VariableShape::multiply_rate_by(double multiplier)
{
	if(multiplier < 0)
		multiplier = - multiplier;
	double p = proportion();

	if(fixed_rate_mode)
	{
		rate = multiplier * rate;
		calc_period();
	}
	else if(multiplier != 0)
		period = period / multiplier;
	//This will keep the  Shape value unchanged
	last_change_time = int(run_time() - p * 1000.0 * period);
}
void VariableShape::set_rate(double seconds)
{
	set_rate(::distance(limit1, limit2), seconds);
}
void VariableShape::set_rate(double dist, double seconds)
{
	fixed_rate_mode = true;
	if(seconds <= 0)
	{
		if(seconds == 0)
			seconds = .001;
		else
			seconds = - seconds;
	}
	if(dist < 0)
		dist = -dist;
	rate = dist / seconds;
	calc_period();
}

double VariableShape::distance()
{
	return ::distance(old_val, new_val);
}
int VariableShape::percent()
{
	return int(proportion() * 100.0);
}
double VariableShape::proportion()
{
	if(period == 0)
		return 0;
	double p = (run_time() - last_change_time) / (1000.0 * period);
	if(p > 1)
	{
		end();
		return 1.0;
	}
	else
		return p;
}
void VariableShape::calc_period()
{
	if(!fixed_rate_mode)
		return;
	if(rate == 0)
		set_period(0);
	else
	{
		period = distance() / rate;
	}
}
/////////////////////////////////////////////////////////////////////////////

RoundedShape::RoundedShape(int x_, int y_, int rx_, int ry_, int n_, int out_)
:Shape(x_, y_, rx_, ry_, n_, 0, out_)
{

}
RoundedShape::RoundedShape(Point center, int rx_, int ry_, int n_, int out_)
:Shape(center, rx_, ry_, n_, 0, out_)
{

}
RoundedShape::RoundedShape(Point left_top, Point right_bottom, int n_, int out_)
:Shape(left_top, right_bottom, n_, 0, out_)
{

}
RoundedShape::RoundedShape(Shape s)
:Shape(s.x, s.y, s.rx, s.ry, s.num_sides, 0, s.out)
{

}
void RoundedShape::draw()
{
	if( (num_sides < 3) || (out == 0) )
	{
		draw_ellipse(x, y, rx, ry);
		return;
	}
	if(out == 100)
	{
		draw_elliptic_outgon(x, y, rx, ry, num_sides);
		return;
	}
	if(out == -100)
	{
		draw_elliptic_ngon(x, y, rx, ry, num_sides);
		return;
	}
	int new_rx, new_ry, new_out;

	Point* verts = new Point[num_sides];
	Point* behinds = new Point[num_sides];
	Point* aheads = new Point[num_sides];
	get_parameters(new_rx, new_ry, new_out, verts, behinds, aheads);
	int i;
	for(i = 0; i < num_sides; i++)
		//This prevents the drawing of a full ellipse since equal pt.
		//calls to elliptic_arc will generate such.
		if(behinds[i] != aheads[i])
			draw_elliptic_arc(x, y, new_rx, new_ry, behinds[i], aheads[i]);
	for(i = 0; i < num_sides - 1; i++)
		draw_line(aheads[i], behinds[i + 1]);	
	draw_line(aheads[num_sides - 1], behinds[0]);
	delete [] aheads;
	delete [] behinds;
	delete [] verts;
}
void RoundedShape::draw_filled()
{
	if( (num_sides < 3) || (out == 0) )
	{
		draw_ellipse_filled(x, y, rx, ry);
		return;
	}
	if(out == 100)
	{
		draw_elliptic_outgon_filled(x, y, rx, ry, num_sides);
		return;
	}
	if(out == -100)
	{
		draw_elliptic_ngon_filled(x, y, rx, ry, num_sides);
	}
	int new_rx, new_ry, new_out;

	Point* verts = new Point[num_sides];
	Point* behinds = new Point[num_sides];
	Point* aheads = new Point[num_sides];
	get_parameters(new_rx, new_ry, new_out, verts, behinds, aheads);

	Point* new_verts = new Point[2 * num_sides];
	int i;
	for(i = 0; i < num_sides; i++)
	{
		draw_elliptic_arc_filled(x, y, new_rx, new_ry, behinds[i], aheads[i]);
		new_verts[2 * i] = behinds[i];
		new_verts[2 * i + 1] = aheads[i];
	}

	draw_polygon_filled_f(new_verts, 2 * num_sides);

	delete [] new_verts;
	delete [] aheads;
	delete [] behinds;
	delete [] verts;
}
void RoundedShape::draw_3D()
{
	Color save_color = get_pen_color();
	set_pen_color(get_3D_color());
	int depth;
	int new_x, new_y;
	int new_rx, new_ry;
	
	for(depth = 1; depth <= get_3D_depth(); depth += get_3D_spacing())
	{
		new_rx = rx + depth * rx * get_3D_flare() / (get_3D_depth() * 45);
		new_ry = ry + depth * ry * get_3D_flare() / (get_3D_depth() * 45);
		new_x = x + (depth * get_3D_angle_x()) / 45;
		new_y = y + (depth * get_3D_angle_y()) / 45;
		RoundedShape(new_x, new_y, new_rx, new_ry, num_sides, out).draw();
	}
	set_pen_color(save_color);
	draw();
}
void RoundedShape::draw_3D_filled()
{
	draw_3D();
	draw_filled();
}
void RoundedShape::draw_P3D()
{
	Color save_color = get_pen_color();
	set_pen_color(get_3D_color());
	
	int depth;
	double dist_to_vp = distance(x, y, get_3D_vanishing_point());
	int max_depth = get_3D_depth();
	if( dist_to_vp < get_3D_depth())
		max_depth = int( dist_to_vp );
	
	Point new_center;
	int new_rx, new_ry;

	for(depth = 1; depth <= max_depth; depth += get_3D_spacing())
	{
		new_center = circle_point(x, y, depth, get_3D_vanishing_point());
		new_rx = int( double(rx) * distance(VanPt, new_center) / dist_to_vp);
		new_ry = int( double(ry) * distance(VanPt, new_center) / dist_to_vp);
		
		RoundedShape(new_center, new_rx, new_ry, num_sides, out).draw();
	}
	set_pen_color(save_color);
	draw();
}
void RoundedShape::draw_P3D_filled()
{
	draw_P3D();
	draw_filled();
}
void RoundedShape::get_parameters(int& new_rx, int& new_ry, int& new_out, 
								  Point* verts, Point* behinds, Point* aheads)
{
	new_rx = rx;
	new_ry = ry;
	new_out = out;
	double scale = cos(PI / num_sides);
	if(out > 0)
	{
		new_rx = int( rx / (1 + out * (scale - 1) / 100));
		new_ry = int( ry / (1 + out * (scale - 1) / 100));
		new_out = -out;
	}
	vertices_ngon(verts, x, y, new_rx, num_sides, rot);

	int i;
	for(i = 0; i < num_sides; i++)
	{
		double angle = circle_angle(x, y, verts[i]);
		double offset = 180 * (100 + new_out) / ( num_sides * 100.0);
		behinds[i] = circle_point(x, y, new_rx, int(angle - offset));
		aheads[i] = circle_point(x, y, new_rx, int(angle + offset));
		if(new_rx > 0)
		{
			behinds[i].y = y +(behinds[i].y - y) * new_ry / new_rx;
			aheads[i].y = y +(aheads[i].y - y) * new_ry / new_rx;
		}
	}
}

///////////////////////////////////////////////////////////////////////////
//*****************************************************************************
//*************************THE VariableRoundedShape CLASS**********************
//*****************************************************************************
VariableRoundedShape::VariableRoundedShape(double seconds, RoundedShape lim1, RoundedShape lim2, bool is_fixed_rate)
{
	limit1 = lim1;
	limit2 = lim2;
	old_val = random_rounded_shape(limit1, limit2);
	new_val = random_rounded_shape(limit1, limit2);
	fixed_rate_mode = is_fixed_rate;
	if(seconds <= 0)
		seconds = .001;
	if(fixed_rate_mode)	//seconds specifies the time between limits, determining rate
	{
		rate = ::distance(limit1, limit2) / seconds;
		calc_period();
	}
	else	//seconds now gives the fixed period
	{
		period = seconds;
		//if not in fixed rate mode, then rate gotten from get_rate();
	}
	last_change_time = run_time();
}
VariableRoundedShape::VariableRoundedShape(double seconds, bool is_fixed_rate)
{
	*this = VariableRoundedShape(seconds, RoundedShape(PerX(25),PerY(25),PerX(5),PerY(5),3,-100), RoundedShape(PerX(75),PerY(75),PerX(35),PerY(35),8,100),is_fixed_rate);
}

//Conversion operator allows VariableRoundedShapes to be used as RoundedShape.
VariableRoundedShape::operator RoundedShape()
{
	if(run_time() - last_change_time > period * 1000)
	{
		old_val = new_val;
		new_val = random_rounded_shape(limit1, limit2);
		last_change_time = run_time();
	}
	if(fixed_rate_mode)
		calc_period();
	if(period <= 0)
		period = .001;
	return range(old_val, new_val, 
		 (run_time() - last_change_time) / (1000.0 * period));
}

void VariableRoundedShape::begin()
{
	last_change_time = run_time();
}
void VariableRoundedShape:: end()
{
	old_val = new_val;
	new_val = random_rounded_shape(limit1, limit2);
	last_change_time = run_time();
	if(fixed_rate_mode)
		calc_period();
}
void VariableRoundedShape::reset()
{
	old_val = random_rounded_shape(limit1, limit2);
	new_val = random_rounded_shape(limit1, limit2);
	last_change_time = run_time();
	if(fixed_rate_mode)
		calc_period();
}
void VariableRoundedShape::reset(RoundedShape start)
{
	old_val = start;
	new_val = random_rounded_shape(limit1, limit2);
	last_change_time = run_time();
	if(fixed_rate_mode)
		calc_period();
}
void VariableRoundedShape::reverse()
{
	RoundedShape temp = new_val;
	new_val = old_val;
	old_val = temp;
	last_change_time = int(2 * run_time() - last_change_time - 1000 * period);
}

//Change the limits of possible values
//Note that we will still finish the present transition
void VariableRoundedShape::set_limits(RoundedShape lim1, RoundedShape lim2, bool reset_it)
{
	limit1 = lim1;
	limit2 = lim2;
	if(fixed_rate_mode)
		calc_period();
	if(reset_it)
		reset();
}

//How long between changes?
double VariableRoundedShape::get_period()
{
	return period;
}
void VariableRoundedShape::set_period(double seconds)
{
	fixed_rate_mode = false;
	period = seconds;
}
double VariableRoundedShape::get_rate()
{
	if(fixed_rate_mode)
		return rate;
	else
	{
		double d = distance();
		if(d == 0 || period == 0)
			return 0;
		else
			return d / period;
	}
}
void VariableRoundedShape::multiply_rate_by(double multiplier)
{
	if(multiplier < 0)
		multiplier = - multiplier;
	double p = proportion();

	if(fixed_rate_mode)
	{
		rate = multiplier * rate;
		calc_period();
	}
	else if(multiplier != 0)
		period = period / multiplier;
	//This will keep the  RoundedShape value unchanged
	last_change_time = int(run_time() - p * 1000.0 * period);
}
void VariableRoundedShape::set_rate(double seconds)
{
	set_rate(::distance(limit1, limit2), seconds);
}
void VariableRoundedShape::set_rate(double dist, double seconds)
{
	fixed_rate_mode = true;
	if(seconds <= 0)
	{
		if(seconds == 0)
			seconds = .001;
		else
			seconds = - seconds;
	}
	if(dist < 0)
		dist = -dist;
	rate = dist / seconds;
	calc_period();
}

double VariableRoundedShape::distance()
{
	return ::distance(old_val, new_val);
}
int VariableRoundedShape::percent()
{
	return int(proportion() * 100.0);
}
double VariableRoundedShape::proportion()
{
	if(period == 0)
		return 0;
	double p = (run_time() - last_change_time) / (1000.0 * period);
	if(p > 1)
	{
		end();
		return 1.0;
	}
	else
		return p;
}
void VariableRoundedShape::calc_period()
{
	if(!fixed_rate_mode)
		return;
	if(rate == 0)
		set_period(0);
	else
	{
		period = distance() / rate;
	}
}
//*****************************************************************************
//*************************THE VariableXXX CLASSES*****************************
//*****************************************************************************
VariableColor::VariableColor()
{
	(*this) = VariableColor(5);
}
VariableColor::VariableColor(double seconds, Color lim1, Color lim2, bool is_fixed_rate)
{
	limit1 = lim1;
	limit2 = lim2;
	old_val = random_color(limit1, limit2);
	new_val = random_color(limit1, limit2);
	fixed_rate_mode = is_fixed_rate;
	if(seconds <= 0)
		seconds = .001;
	if(fixed_rate_mode)	//seconds specifies the time between limits, determining rate
	{
		rate = ::distance(limit1, limit2) / seconds;
		calc_period();
	}
	else	//seconds now gives the fixed period
	{
		period = seconds;
		//if not in fixed rate mode, then rate gotten from get_rate();
	}
	last_change_time = run_time();
}
VariableColor::VariableColor(double seconds, bool is_fixed_rate)
{
	*this = VariableColor(seconds, BLACK, WHITE, is_fixed_rate);
}

//Conversion operator allows VariableColors to be used as Color.
VariableColor::operator Color()
{
	if(run_time() - last_change_time > period * 1000)
	{
		old_val = new_val;
		new_val = random_color(limit1, limit2);
		last_change_time = run_time();
	}
	if(fixed_rate_mode)
		calc_period();
	if(period <= 0)
		period = .001;
	return range(old_val, new_val, 
		 (run_time() - last_change_time) / (1000.0 * period));
}

void VariableColor::begin()
{
	last_change_time = run_time();
}
void VariableColor:: end()
{
	old_val = new_val;
	new_val = random_color(limit1, limit2);
	last_change_time = run_time();
	if(fixed_rate_mode)
		calc_period();
}
void VariableColor::reset()
{
	old_val = random_color(limit1, limit2);
	new_val = random_color(limit1, limit2);
	last_change_time = run_time();
	if(fixed_rate_mode)
		calc_period();
}
void VariableColor::reset(Color start)
{
	old_val = start;
	new_val = random_color(limit1, limit2);
	last_change_time = run_time();
	if(fixed_rate_mode)
		calc_period();
}
void VariableColor::reverse()
{
	Color temp = new_val;
	new_val = old_val;
	old_val = temp;
	last_change_time = int(2 * run_time() - last_change_time - 1000 * period);
}

//Change the limits of possible values
//Note that we will still finish the present transition
void VariableColor::set_limits(Color lim1, Color lim2, bool reset_it)
{
	limit1 = lim1;
	limit2 = lim2;
	if(fixed_rate_mode)
		calc_period();
	if(reset_it)
		reset();
}

//How long between changes?
double VariableColor::get_period()
{
	return period;
}
void VariableColor::set_period(double seconds)
{
	fixed_rate_mode = false;
	period = seconds;
}
double VariableColor::get_rate()
{
	if(fixed_rate_mode)
		return rate;
	else
	{
		double d = distance();
		if(d == 0 || period == 0)
			return 0;
		else
			return d / period;
	}
}
void VariableColor::multiply_rate_by(double multiplier)
{
	if(multiplier < 0)
		multiplier = - multiplier;
	double p = proportion();

	if(fixed_rate_mode)
	{
		rate = multiplier * rate;
		calc_period();
	}
	else if(multiplier != 0)
		period = period / multiplier;
	//This will keep the  Color value unchanged
	last_change_time = int(run_time() - p * 1000.0 * period);
}
void VariableColor::set_rate(double seconds)
{
	set_rate(::distance(limit1, limit2), seconds);
}
void VariableColor::set_rate(double dist, double seconds)
{
	fixed_rate_mode = true;
	if(seconds <= 0)
	{
		if(seconds == 0)
			seconds = .001;
		else
			seconds = - seconds;
	}
	if(dist < 0)
		dist = -dist;
	rate = dist / seconds;
	calc_period();
}

double VariableColor::distance()
{
	return ::distance(old_val, new_val);
}
int VariableColor::percent()
{
	return int(proportion() * 100.0);
}
double VariableColor::proportion()
{
	if(period == 0)
		return 0;
	double p = (run_time() - last_change_time) / (1000.0 * period);
	if(p > 1)
	{
		end();
		return 1.0;
	}
	else
		return p;
}
void VariableColor::calc_period()
{
	if(!fixed_rate_mode)
		return;
	if(rate == 0)
		set_period(0);
	else
	{
		period = distance() / rate;
	}
}
//////////////////////////////////////////////
VariableDouble::VariableDouble()
{

}
VariableDouble::VariableDouble(double seconds, double low, double high, bool is_fixed_rate)
{
	limit1 = low;
	limit2 = high;
	old_val = random_continuous(limit1, limit2);
	new_val = random_continuous(limit1, limit2);
	fixed_rate_mode = is_fixed_rate;
	if(seconds <= 0)
		seconds = .001;
	if(fixed_rate_mode)	//seconds specifies the time between limits, determining rate
	{
		rate = fabs(limit2 - limit1) / seconds;
		calc_period();
	}
	else	//seconds now gives the fixed period
	{
		period = seconds;
		//if not in fixed rate mode, then rate gotten from get_rate();
	}
	last_change_time = run_time();
}
VariableDouble::VariableDouble(double seconds, bool is_fixed_rate)
{
	*this = VariableDouble(seconds, 0.0, 1.0, is_fixed_rate);
}

//Conversion operator allows VariableDoubles to be used as int.
VariableDouble::operator double()
{
	if(run_time() - last_change_time > period * 1000)
	{
		old_val = new_val;
		new_val = random_continuous(limit1, limit2);
		last_change_time = run_time();
	}
	if(fixed_rate_mode)
		calc_period();
	if(period <= 0)
		period = .001;
	return range_continuous(old_val, new_val, 
		 (run_time() - last_change_time) / (1000.0 * period));
}

void VariableDouble::begin()
{
	last_change_time = run_time();
}
void VariableDouble:: end()
{
	old_val = new_val;
	new_val = random_continuous(limit1, limit2);
	last_change_time = run_time();
	if(fixed_rate_mode)
		calc_period();
}
void VariableDouble::reset()
{
	old_val = random_continuous(limit1, limit2);
	new_val = random_continuous(limit1, limit2);
	last_change_time = run_time();
	if(fixed_rate_mode)
		calc_period();
}
void VariableDouble::reset(double start)
{
	old_val = start;
	new_val = random_continuous(limit1, limit2);
	last_change_time = run_time();
	if(fixed_rate_mode)
		calc_period();
}
void VariableDouble::reverse()
{
	double temp = new_val;
	new_val = old_val;
	old_val = temp;
	last_change_time = int(2 * run_time() - last_change_time - 1000 * period);
}
//Change the limits of possible values
//Note that we will still finish the present path
void VariableDouble::set_limits(double low, double high, bool reset_it)
{
	limit1 = low;
	limit2 = high;
	if(fixed_rate_mode)
		calc_period();
	if(reset_it)
		reset();
}

//How long between changes?
double VariableDouble::get_period()
{
	return period;
}
void VariableDouble::set_period(double seconds)
{
	fixed_rate_mode = false;
	period = seconds;
}
double VariableDouble::get_rate()
{
	if(fixed_rate_mode)
		return rate;
	else
	{
		double d = distance();
		if(d == 0 || period == 0)
			return 0;
		else
			return d / period;
	}
}
void VariableDouble::multiply_rate_by(double multiplier)
{
	if(multiplier < 0)
		multiplier = - multiplier;
	double p = proportion();

	if(fixed_rate_mode)
	{
		rate = multiplier * rate;
		calc_period();
	}
	else if(multiplier != 0)
		period = period / multiplier;
	//This will keep the  int in the same place:
	last_change_time = int(run_time() - p * 1000.0 * period);
}
void VariableDouble::set_rate(double seconds)
{
	set_rate(abs(limit2 - limit1), seconds);
}
void VariableDouble::set_rate(double dist, double seconds)
{
	fixed_rate_mode = true;
	if(seconds <= 0)
	{
		if(seconds == 0)
			seconds = .001;
		else
			seconds = - seconds;
	}
	if(dist < 0)
		dist = -dist;
	rate = dist / seconds;
	calc_period();
}

double VariableDouble::difference()
{
	return new_val - old_val;
}
double VariableDouble::distance()
{
	return fabs(old_val - new_val);
}
int VariableDouble::percent()
{
	return int(proportion() * 100.0);
}
double VariableDouble::proportion()
{
	if(period == 0)
		return 0;
	double p = (run_time() - last_change_time) / (1000.0 * period);
	if(p > 1)
	{
		end();
		return 1.0;
	}
	else
		return p;
}
double VariableDouble::low()
{
	return limit1;
}
double VariableDouble::high()
{
	return limit2;
}
void VariableDouble::calc_period()
{
	if(!fixed_rate_mode)
		return;
	if(rate == 0)
		set_period(0);
	else
	{
		period = distance() / rate;
	}
}
/////////////////////////////////////////////////////////////////

VariableInt::VariableInt()
{
	(*this) = VariableInt(5, 0, 100);
}
VariableInt::VariableInt(double seconds, int low, int high, bool is_fixed_rate)
{
	limit1 = low;
	limit2 = high;
	old_val = random(limit1, limit2);
	new_val = random(limit1, limit2);
	fixed_rate_mode = is_fixed_rate;
	if(seconds <= 0)
		seconds = .001;
	if(fixed_rate_mode)	//seconds specifies the time between limits, determining rate
	{
		rate = abs(limit2 - limit1) / seconds;
		calc_period();
	}
	else	//seconds now gives the fixed period
	{
		period = seconds;
		//if not in fixed rate mode, then rate gotten from get_rate();
	}
	last_change_time = run_time();
}
VariableInt::VariableInt(double seconds = 5.0, bool is_fixed_rate = false)
{
	*this = VariableInt(seconds, 0, 100, is_fixed_rate);
}

//Conversion operator allows VariableInts to be used as int.
VariableInt::operator int()
{
	if(run_time() - last_change_time > period * 1000)
	{
		old_val = new_val;
		new_val = random(limit1, limit2);
		last_change_time = run_time();
	}
	if(fixed_rate_mode)
		calc_period();
	if(period <= 0)
		period = .001;
	return range(old_val, new_val, 
		 (run_time() - last_change_time) / (1000.0 * period));
}

void VariableInt::begin()
{
	last_change_time = run_time();
}
void VariableInt:: end()
{
	old_val = new_val;
	new_val = random(limit1, limit2);
	last_change_time = run_time();
	if(fixed_rate_mode)
		calc_period();
}
void VariableInt::reset()
{
	old_val = random(limit1, limit2);
	new_val = random(limit1, limit2);
	last_change_time = run_time();
	if(fixed_rate_mode)
		calc_period();
}
void VariableInt::reset(int start)
{
	old_val = start;
	new_val = random(limit1, limit2);
	last_change_time = run_time();
	if(fixed_rate_mode)
		calc_period();
}
void VariableInt::reverse()
{
	int temp = new_val;
	new_val = old_val;
	old_val = temp;
	last_change_time = int(2 * run_time() - last_change_time - 1000 * period);
}
//Change the limits of possible values
//Note that we will still finish the present path
void VariableInt::set_limits(int low, int high, bool reset_it)
{
	limit1 = low;
	limit2 = high;
	if(fixed_rate_mode)
		calc_period();
	if(reset_it)
		reset();
}

//How long between changes?
double VariableInt::get_period()
{
	return period;
}
void VariableInt::set_period(double seconds)
{
	fixed_rate_mode = false;
	period = seconds;
}
double VariableInt::get_rate()
{
	if(fixed_rate_mode)
		return rate;
	else
	{
		double d = distance();
		if(d == 0 || period == 0)
			return 0;
		else
			return d / period;
	}
}
void VariableInt::multiply_rate_by(double multiplier)
{
	if(multiplier < 0)
		multiplier = - multiplier;
	double p = proportion();

	if(fixed_rate_mode)
	{
		rate = multiplier * rate;
		calc_period();
	}
	else if(multiplier != 0)
		period = period / multiplier;
	//This will keep the  int in the same place:
	last_change_time = int(run_time() - p * 1000.0 * period);
}
void VariableInt::set_rate(double seconds)
{
	set_rate(abs(limit2 - limit1), seconds);
}
void VariableInt::set_rate(double dist, double seconds)
{
	fixed_rate_mode = true;
	if(seconds <= 0)
	{
		if(seconds == 0)
			seconds = .001;
		else
			seconds = - seconds;
	}
	if(dist < 0)
		dist = -dist;
	rate = dist / seconds;
	calc_period();
}

int VariableInt::difference()
{
	return new_val - old_val;
}
double VariableInt::distance()
{
	return abs(old_val - new_val);
}
int VariableInt::percent()
{
	return int(proportion() * 100.0);
}
double VariableInt::proportion()
{
	if(period == 0)
		return 0;
	double p = (run_time() - last_change_time) / (1000.0 * period);
	if(p > 1)
	{
		end();
		return 1.0;
	}
	else
		return p;
}
int VariableInt::low()
{
	return limit1;
}
int VariableInt::high()
{
	return limit2;
}
void VariableInt::calc_period()
{
	if(!fixed_rate_mode)
		return;
	if(rate == 0)
		set_period(0);
	else
	{
		period = distance() / rate;
	}
}
////////////////////////////////////////////////////////////////////////////////////
VariablePoint::VariablePoint()
:limit1(0,0), limit2(1024,768), 	
 old_val(random_point(limit1, limit2)), new_val(random_point(limit1, limit2)),
 fixed_rate_mode(false), period(5), last_change_time(0)
{
}
VariablePoint::VariablePoint(double seconds, Point left_top, Point right_bottom, bool is_fixed_rate)
{
	limit1 = left_top;
	limit2 = right_bottom;
	old_val = random_point(limit1, limit2);
	new_val = random_point(limit1, limit2);
	fixed_rate_mode = is_fixed_rate;
	if(seconds <= 0)
		seconds = .001;
	if(fixed_rate_mode)	//seconds specifies the time between limits, determining rate
	{
		rate = ::distance(left_top, right_bottom) / seconds;
		calc_period();
	}
	else	//seconds now gives the fixed period
	{
		period = seconds;
		//if not in fixed rate mode, then rate gotten from get_rate();
	}
	last_change_time = run_time();
}
VariablePoint::VariablePoint(double seconds, bool is_fixed_rate)
{
	*this = VariablePoint(seconds, MinPt, MaxPt, is_fixed_rate);
}

//Conversion operator allows VariablePoints to be used as Point.
VariablePoint::operator Point()
{
	if(run_time() - last_change_time > period * 1000)
	{
		old_val = new_val;
		new_val = random_point(limit1, limit2);
		last_change_time = run_time();
	}
	if(fixed_rate_mode)
		calc_period();
	if(period <= 0)
		period = .001;
	return range(old_val, new_val, 
		 (run_time() - last_change_time) / (1000.0 * period));
}
int VariablePoint::x()
{
	return Point(*this).x;
}
int VariablePoint::y()
{
	return Point(*this).y;
}
void VariablePoint::begin()
{
	last_change_time = run_time();
}
void VariablePoint:: end()
{
	old_val = new_val;
	new_val = random_point(limit1, limit2);
	last_change_time = run_time();
	if(fixed_rate_mode)
		calc_period();
}
void VariablePoint::reset()
{
	old_val = random_point(limit1, limit2);
	new_val = random_point(limit1, limit2);
	last_change_time = run_time();
	if(fixed_rate_mode)
		calc_period();
}
void VariablePoint::reset(Point start)
{
	old_val = start;
	new_val = random_point(limit1, limit2);
	last_change_time = run_time();
	if(fixed_rate_mode)
		calc_period();
}
void VariablePoint::reverse()
{
	Point temp = new_val;
	new_val = old_val;
	old_val = temp;
	last_change_time = int(2 * run_time() - last_change_time - 1000 * period);
}
void VariablePoint::show_path()
{
	draw_line(old_val, new_val);
}

//Change the limits of possible values
//Note that we will still finish the present path
void VariablePoint::set_limits(int x, int y, int rx, int ry, bool reset_it)
{
	set_limits(Point(x - rx, y - ry), Point(x + rx, y + ry));
	if(reset_it)
		reset();
}
void VariablePoint::set_limits(Point center, int rx, int ry, bool reset_it)
{
	set_limits(center.x, center.y, rx, ry);
	if(reset_it)
		reset();
}
void VariablePoint::set_limits(Point left_top, Point right_bottom, bool reset_it)
{
	limit1 = left_top;
	limit2 = right_bottom;
	if(fixed_rate_mode)
		calc_period();
	if(reset_it)
		reset();
}
void VariablePoint::set_limits(int inset, bool reset_it)
{
	set_limits(inset, inset);
	if(reset_it)
		reset();
}
void VariablePoint::set_limits(int inset_x, int inset_y, bool reset_it)
{
	set_limits(MinPt + Point(inset_x, inset_y), MaxPt - Point(inset_x, inset_y));
	if(reset_it)
		reset();
}

//How long between changes?
double VariablePoint::get_period()
{
	return period;
}
void VariablePoint::set_period(double seconds)
{
	fixed_rate_mode = false;
	period = seconds;
}
double VariablePoint::get_rate()
{
	if(fixed_rate_mode)
		return rate;
	else
	{
		double d = distance();
		if(d == 0 || period == 0)
			return 0;
		else
			return d / period;
	}
}
void VariablePoint::multiply_rate_by(double multiplier)
{
	if(multiplier < 0)
		multiplier = - multiplier;
	double p = proportion();

	if(fixed_rate_mode)
	{
		rate = multiplier * rate;
		calc_period();
	}
	else if(multiplier != 0)
		period = period / multiplier;
	//This will keep the  point in the same place:
	last_change_time = int(run_time() - p * 1000.0 * period);
}
void VariablePoint::set_rate(double seconds)
{
	set_rate(::distance(limit1, limit2), seconds);
}
void VariablePoint::set_rate(double dist, double seconds)
{
	fixed_rate_mode = true;
	if(seconds <= 0)
	{
		if(seconds == 0)
			seconds = .001;
		else
			seconds = - seconds;
	}
	if(dist < 0)
		dist = -dist;
	rate = dist / seconds;
	calc_period();
}

Point VariablePoint::difference()
{
	return new_val - old_val;
}
double VariablePoint::distance()
{
	return ::distance(old_val, new_val);
}
int VariablePoint::percent()
{
	return int(proportion() * 100.0);
}
double VariablePoint::proportion()
{
	if(period == 0)
		return 0;
	double p = (run_time() - last_change_time) / (1000.0 * period);
	if(p > 1)
	{
		end();
		return 1.0;
	}
	else
		return p;
}
Point VariablePoint::center()
{
	return Point((limit1.x + limit2.x) / 2, (limit1.y + limit2.y) / 2);
}
int VariablePoint::rx()
{
	return (limit2.x - limit1.x) / 2;
}
int VariablePoint::ry()
{
	return (limit2.y - limit1.y) / 2;
}
int VariablePoint::left()
{
	return limit1.x;
}
int VariablePoint::right()
{
	return limit2.x;
}
int VariablePoint::top()
{
	return limit1.y;
}
int VariablePoint::bottom()
{
	return limit2.y;
}
Point VariablePoint::left_top()
{
	return limit1;
}
Point VariablePoint::right_bottom()
{
	return limit2;
}

void VariablePoint::calc_period()
{
	if(!fixed_rate_mode)
		return;
	if(rate == 0)
		set_period(0);
	else
	{
		period = distance() / rate;
	}
}
//*****************************************************************************
//*************************Free Functions**************************************
//*****************************************************************************
bool animation_between(int start_percent, int stop_percent)
{
	return (start_percent <= animation_percent()) &&
		   (animation_percent() <= stop_percent);
}
void animation_display()
{
	Animator::current -> animate();
}
void animation_end()
{
	Animator::current -> end();
}
int animation_percent()
{
	return int(Animator::current -> proportion() * 100.0);
}
double animation_proportion()
{
	return Animator::current -> proportion();
}
double circle_angle(int x, int y, int rayPt_x, int rayPt_y)
{
	if(distance(x,y,rayPt_x,rayPt_y) == 0)
		return 0;
	double rad;
	if(rayPt_y <= y)
		rad = acos(double(rayPt_x-x)/distance(x,y,rayPt_x,rayPt_y));
	else
		rad = 2*PI - acos(double(rayPt_x-x)/distance(x,y,rayPt_x,rayPt_y));
	return rad * RAD2DEG;
}
double circle_angle(Point center, int rayPt_x, int rayPt_y)
{
	return circle_angle(center.x, center.y, rayPt_x, rayPt_y);
}
double circle_angle(int x, int y, Point rayPt)
{
	return circle_angle(x, y, rayPt.x, rayPt.y);
}
double circle_angle(Point center, Point rayPt)
{
	return circle_angle(center.x, center.y, rayPt.x, rayPt.y);
}
Point circle_point(int x, int y, int r, int angle)
{
	return Point(int(x + r * cos( angle * DEG2RAD)),
		         int(y - r * sin( angle * DEG2RAD)));
}
Point circle_point(Point center, int r, int angle)
{
	return circle_point(center.x, center.y, r, angle);
}
Point circle_point(int x, int y, int r, Point ray_point)
{
	return circle_point(x, y, r, int(circle_angle(x, y, ray_point)));
}
Point circle_point(Point center, int r, Point ray_point)
{
	return circle_point(center.x, center.y, r, ray_point);
}
void close_window()
{
	if(gfxe_hwnd == NULL)
		return;

	DeleteObject(gfxe_background_brush);

	int c;
	for(c = 1; c < gfxe_NUMPAGES; c++)
	{
		HBITMAP hbitmap = (HBITMAP) GetCurrentObject(gfxe_hdc[c], OBJ_BITMAP);
		DeleteDC(gfxe_hdc[c]);
		DeleteObject(hbitmap);
	}

	HPEN hpen = (HPEN) GetCurrentObject(gfxe_hdc[0], OBJ_PEN);
	HBRUSH hbrush = (HBRUSH) GetCurrentObject(gfxe_hdc[0], OBJ_BRUSH);
	HFONT hfont = (HFONT) GetCurrentObject(gfxe_hdc[0], OBJ_FONT);
	ReleaseDC(gfxe_hwnd, gfxe_hdc[0]);
	DeleteObject(hpen);
	DeleteObject(hbrush);
	DeleteObject(hfont);

	//The DestroyWindow function destroys the specified window. The function
	//sends a WM_DESTROY message to the window to deactivate it and remove the
	//keyboard focus from it. The function also destroys the window's menu,
	//flushes the thread message queue, and destroys timers, 
	DestroyWindow(gfxe_hwnd);	//Post a WM_DESTROY message
	gfxe_hwnd = NULL;
}
void copy_block(Point left_top_src, Point right_bottom_src, Point left_top_dest, int src_page, int dest_page)
{
	if(src_page < 0 || dest_page < 0)
		src_page = dest_page = gfx_active_page;

	BitBlt(gfxe_hdc[dest_page], left_top_dest.x, left_top_dest.y, right_bottom_src.x - left_top_src.x + 1, right_bottom_src.y - left_top_src.y + 1,
		   gfxe_hdc[src_page], left_top_src.x, left_top_src.y, SRCCOPY);
	if(dest_page == 0)
		BitBlt(gfxe_hdc[3], left_top_dest.x, left_top_dest.y, right_bottom_src.x - left_top_src.x + 1, right_bottom_src.y - left_top_src.y + 1,
		   gfxe_hdc[src_page], left_top_src.x, left_top_src.y, SRCCOPY);
}
void copy_block_stretch(Point left_top_src,  Point right_bottom_src, Point left_top_dest, Point right_bottom_dest, int src_page, int dest_page)
{
	if(src_page < 0 || dest_page < 0)
		src_page = dest_page = gfx_active_page;
	StretchBlt(gfxe_hdc[dest_page], left_top_dest.x, left_top_dest.y, right_bottom_dest.x - left_top_dest.x + 1, right_bottom_dest.y - left_top_dest.y +1,
		   gfxe_hdc[src_page], left_top_src.x, left_top_src.y, right_bottom_src.x -  left_top_src.x + 1, right_bottom_src.y - left_top_src.y + 1, SRCCOPY);
	if(gfx_active_page == 0)
		StretchBlt(gfxe_hdc[3], left_top_dest.x, left_top_dest.y, right_bottom_dest.x - left_top_dest.x + 1, right_bottom_dest.y - left_top_dest.y +1,
		   gfxe_hdc[src_page], left_top_src.x, left_top_src.y, right_bottom_src.x -  left_top_src.x + 1, right_bottom_src.y - left_top_src.y + 1, SRCCOPY);
}
void copy_buffer_to_screen() {copy_page(1,0);}
void copy_buffer2_to_buffer(){copy_page(2,1);}
void copy_buffer2_to_screen(){copy_page(2,0);}
void copy_page(int source_page, int destination_page)
{
	BitBlt(gfxe_hdc[destination_page],0,0,screen_width(), screen_height(),
		   gfxe_hdc[source_page],0,0,SRCCOPY);
	if(destination_page == 0)
		BitBlt(gfxe_hdc[3],0,0, screen_width(), screen_height(),
			   gfxe_hdc[source_page],0,0,SRCCOPY);
}
Point cursor_point()
{
	MSG msg;
	//Process everything in the message queue.  
	if(PeekMessage(&msg, gfxe_hwnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//We post a phony message to retrieve it and take the cursor position.
	//Note that the cursor position is in screen coordinates, not window coordinates
	PostMessage(gfxe_hwnd, WM_USER, 0, 0);
	if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	int x = msg.pt.x  - GetSystemMetrics(SM_CXBORDER) - 3;
	if(x > MaxX) x = MaxX;
	if(x < 0) x = 0;
	int y = msg.pt.y  - GetSystemMetrics(SM_CYCAPTION)-GetSystemMetrics(SM_CYBORDER) - 3;
	if(y > MaxY) y = MaxY;
	if(y < 0) y = 0;
	return Point(x,y);
}
Point cursor_point_rescaled(int x, int y, int rx, int ry)
{
	return cursor_point_rescaled(Point(x-rx,y-ry),Point(x+rx,y+ry));
}
Point cursor_point_rescaled(Point center, int rx, int ry)
{
	return cursor_point_rescaled(center.x, center.y, rx, ry);
}
Point cursor_point_rescaled(Point left_top, Point right_bottom)
{
	return Point(cursor_x_rescaled(left_top.x, right_bottom.x),
		         cursor_y_rescaled(left_top.y, right_bottom.y));
}
void cursor_hide()
{
	ShowCursor(false);
}
void cursor_show()
{
	ShowCursor(true);
}
int cursor_x()
{
	return cursor_point().x;
}
int cursor_x_rescaled(int low, int high)
{
	if(cursor_x() >= MaxX)
		return high;
	if(cursor_x() <= MinX)
		return low;
	//The +1 allows for high to be returned for more than just MaxX.
	return int(low + cursor_x()/double(MaxX) * (high - low + 1));
}
double cursor_x_rescaled_continuous(int low, int high)
{
	if(cursor_x() >= MaxX)
		return high;
	if(cursor_x() <= MinX)
		return low;
	return low + cursor_x()/double(MaxX) * (high - low);
}
int cursor_y()
{
	return cursor_point().y;
}
int cursor_y_rescaled(int low, int high)
{
	if(cursor_y() >= MaxY)
		return high;
	//The +1 allows for high to be returned for more than just MaxY.
	return int(low + cursor_y() / double(MaxY) * (high - low + 1));
}
double cursor_y_rescaled_continuous(int low, int high)
{
	if(cursor_y() >= MaxY)
		return high;
	return low + cursor_y() / double(MaxY) * (high - low);
}
void delay(int millisecs)
{
	Sleep(millisecs);
	return;
	//Here is another way I used to do it.
	int Time = GetTickCount();
	while((UINT)GetTickCount() - (UINT)Time < (UINT) millisecs);
}
double distance(int x1, int y1, int x2, int y2)
{
	return sqrt(pow(double(x1 - x2),double(2)) + pow(double(y1 - y2),double(2)));
}
double distance(Point p, Point q)
{
	return sqrt(pow(double(p.x - q.x),double(2)) + pow(double(p.y - q.y),double(2)));
}
double distance(int x1, int y1, Point q)
{
	return sqrt(pow(double(x1 - q.x),double(2)) + pow(double(y1 - q.y),double(2)));
}
double distance(Point p, int x2, int y2)
{
	return sqrt(pow(double(p.x - x2),double(2)) + pow(double(p.y - y2),double(2)));
}
double distance(Color c1, Color c2)
{
	return sqrt(pow(double(c1.r - c2.r),double(2)) + pow(double(c1.g - c2.g),double(2)) + pow(double(c1.b - c2.b),double(2)));
}
double distance(Shape s, Shape t)
{
	return sqrt(pow(double(s.x - t.x),double(2)) + pow(double(s.y - t.y),double(2)) + pow(double(s.rx - t.rx),double(2)) +
		        pow(double(s.ry - t.ry),double(2)) + pow(double(s.num_sides - t.num_sides),double(2)) +
				pow(double(s.rot - t.rot),double(2)) + pow(double(s.out - t.out),double(2)));
}
//Drawing circular arcs
void draw_arc(int x, int y, int r, int start_angle, int stop_angle)
{
	draw_elliptic_arc(x, y, r, r, start_angle, stop_angle);
}
void draw_arc(Point center, int r, int start_angle, int stop_angle)
{
	draw_arc(center.x, center.y, r, start_angle, stop_angle);
}
void draw_arc(int x, int y, int r, Point start_point, Point stop_point)
{
	draw_elliptic_arc(x, y, r, r, start_point, stop_point);
}
void draw_arc(Point center, int r, Point start_point, Point stop_point)
{
	draw_arc(center.x, center.y, r, start_point, stop_point);
}
void draw_arc_filled(int x, int y, int r, int start_angle, int stop_angle)
{
	draw_elliptic_arc_filled(x, y, r, r, start_angle, stop_angle);
}
void draw_arc_filled(Point center, int r, int start_angle, int stop_angle)
{
	draw_arc_filled(center.x, center.y, r, start_angle, stop_angle);
}
void draw_arc_filled(int x, int y, int r, Point start_point, Point stop_point)
{
	draw_elliptic_arc_filled(x, y, r, r, start_point, stop_point);
}
void draw_arc_filled(Point center, int r, Point start_point, Point stop_point)
{
	draw_arc_filled(center.x, center.y, r, start_point, stop_point);
}
void draw_arc_3D(int x, int y, int r, int start_angle, int stop_angle)
{
	draw_elliptic_arc_3D(x, y, r, r, start_angle, stop_angle);
}
void draw_arc_3D(Point center, int r, int start_angle, int stop_angle)
{
	draw_arc_3D(center.x, center.y, r, start_angle, stop_angle);
}
void draw_arc_3D(int x, int y, int r, Point start_point, Point stop_point)
{
	draw_elliptic_arc_3D(x, y, r, r, start_point, stop_point);
}
void draw_arc_3D(Point center, int r, Point start_point, Point stop_point)
{
	draw_arc_3D(center.x, center.y, r, start_point, stop_point);
}
void draw_arc_3D_filled(int x, int y, int r, int start_angle, int stop_angle)
{
	draw_arc_3D(x, y, r, start_angle, stop_angle);
	draw_arc_filled(x, y, r, start_angle, stop_angle);
}
void draw_arc_3D_filled(Point center, int r, int start_angle, int stop_angle)
{
	draw_arc_3D_filled(center.x, center.y, r, start_angle, stop_angle);
}
void draw_arc_3D_filled(int x, int y, int r, Point start_point, Point stop_point)
{
	draw_arc_3D(x, y, r, start_point, stop_point);
	draw_arc_filled(x, y, r, start_point, stop_point);
}
void draw_arc_3D_filled(Point center, int r, Point start_point, Point stop_point)
{
	draw_arc_3D_filled(center.x, center.y, r, start_point, stop_point);
}
void draw_arc_P3D(int x, int y, int r, int start_angle, int stop_angle)
{
	draw_elliptic_arc_P3D(x, y, r, r, start_angle, stop_angle);
}
void draw_arc_P3D(Point center, int r, int start_angle, int stop_angle)
{
	draw_arc_P3D(center.x, center.y, r, start_angle, stop_angle);
}
void draw_arc_P3D(int x, int y, int r, Point start_point, Point stop_point)
{
	draw_elliptic_arc_P3D(x, y, r, r, start_point, stop_point);
}
void draw_arc_P3D(Point center, int r, Point start_point, Point stop_point)
{
	draw_arc_P3D(center.x, center.y, r, start_point, stop_point);
}
void draw_arc_P3D_filled(int x, int y, int r, int start_angle, int stop_angle)
{
	draw_arc_P3D(x, y, r, start_angle, stop_angle);
	draw_arc_filled(x, y, r, start_angle, stop_angle);
}
void draw_arc_P3D_filled(Point center, int r, int start_angle, int stop_angle)
{
	draw_arc_P3D_filled(center.x, center.y, r, start_angle, stop_angle);
}
void draw_arc_P3D_filled(int x, int y, int r, Point start_point, Point stop_point)
{
	draw_arc_P3D(x, y, r, start_point, stop_point);
	draw_arc_filled(x, y, r, start_point, stop_point);
}
void draw_arc_P3D_filled(Point center, int r, Point start_point, Point stop_point)
{
	draw_arc_P3D_filled(center.x, center.y, r, start_point, stop_point);
}
//Drawing bezier curves
void draw_bezier(Point begin, Point c1, Point c2, Point end)
{
	POINT PT[4];
	PT[0].x = begin.x;
	PT[0].y = begin.y;
	PT[1].x = c1.x;
	PT[1].y = c1.y;
	PT[2].x = c2.x;
	PT[2].y = c2.y;
	PT[3].x = end.x;
	PT[3].y = end.y;

	PolyBezier(gfxe_hdc[gfx_active_page], PT, 4);

	if(gfx_active_page == 0)
		PolyBezier(gfxe_hdc[3], PT, 4);
}

void draw_bezier(Point* all_points, int size)
{
	POINT* PT = new POINT[size];
	for(int i = 0; i < size; i++)
	{
		PT[i].x = all_points[i].x;
		PT[i].y = all_points[i].y;
	}
	PolyBezier(gfxe_hdc[gfx_active_page], PT, size);

	if(gfx_active_page == 0)
		PolyBezier(gfxe_hdc[3], PT, size);
	delete[] PT;
}
void draw_bezier(VariablePoint* all_points, int size)
{
	Point* pts = new Point[size];
	copy_array(all_points, pts, size);
	draw_bezier(pts, size);
	delete[] pts;
}

static POINT Point2POINT(Point p)
{
	POINT P;
	P.x = p.x;
	P.y = p.y;
	return P;
}
void draw_bezier(Point* pts, int num_points, Point* c1, Point* c2)
{
	POINT* PT = new POINT[3*num_points - 2];
	PT[0] = Point2POINT(pts[0]);
	for(int i = 1; i < num_points; i++)
	{
		PT[3*i-2] = Point2POINT(c1[i-1]);
		PT[3*i-1] = Point2POINT(c2[i-1]);
		PT[3*i] = Point2POINT(pts[i]);
	}
	PolyBezier(gfxe_hdc[gfx_active_page], PT, 3*num_points - 2);

	if(gfx_active_page == 0)
		PolyBezier(gfxe_hdc[3], PT, 3*num_points - 2);
	delete[] PT;
}

void draw_bezier(VariablePoint* pts, int num_points, VariablePoint* c1, VariablePoint* c2)
{
	POINT* PT = new POINT[3*num_points - 2];
	PT[0] = Point2POINT(pts[0]);
	for(int i = 1; i < num_points; i++)
	{
		PT[3*i-2] = Point2POINT(c1[i-1]);
		PT[3*i-1] = Point2POINT(c2[i-1]);
		PT[3*i] = Point2POINT(pts[i]);
	}
	PolyBezier(gfxe_hdc[gfx_active_page], PT, 3*num_points - 2);

	if(gfx_active_page == 0)
		PolyBezier(gfxe_hdc[3], PT, 3*num_points - 2);
	delete[] PT;
}


void draw_bezier(Point* pts, int num_points, Point c1, Point* c2, int percent)
{
	Point p;
	POINT* PT = new POINT[3*num_points - 2];
	PT[0] = Point2POINT(pts[0]);
	PT[1] = Point2POINT(c1);
	PT[2] = Point2POINT(c2[0]);
	for(int i = 1; i < num_points - 1; i++)
	{
		PT[3*i] = Point2POINT(pts[i]);
		PT[3*i+1] = Point2POINT(pts[i] + 
			Point(percent * (pts[i].x - c2[i-1].x)/100, percent * (pts[i].y - c2[i-1].y)/100));
		PT[3*i+2] = Point2POINT(c2[i]);
	}
	PT[3*num_points-3] = Point2POINT(pts[num_points-1]);

	PolyBezier(gfxe_hdc[gfx_active_page], PT, 3*num_points - 2);

	if(gfx_active_page == 0)
		PolyBezier(gfxe_hdc[3], PT, 3*num_points - 2);
	delete[] PT;
}
void draw_bezier(VariablePoint* pts, int num_points, Point c1,
				 VariablePoint* c2, int percent)
				 {
	Point p;
	POINT* PT = new POINT[3*num_points - 2];
	PT[0] = Point2POINT(pts[0]);
	PT[1] = Point2POINT(c1);
	PT[2] = Point2POINT(c2[0]);
	for(int i = 1; i < num_points - 1; i++)
	{
		PT[3*i] = Point2POINT(pts[i]);
		PT[3*i+1] = Point2POINT(Point(pts[i]) + 
			Point(percent * (Point(pts[i]).x - Point(c2[i-1]).x)/100,
			      percent * (Point(pts[i]).y - Point(c2[i-1]).y)/100));
		PT[3*i+2] = Point2POINT(c2[i]);
	}
	PT[3*num_points-3] = Point2POINT(pts[num_points-1]);

	PolyBezier(gfxe_hdc[gfx_active_page], PT, 3*num_points - 2);

	if(gfx_active_page == 0)
		PolyBezier(gfxe_hdc[3], PT, 3*num_points - 2);
	delete[] PT;

	//Point* ppts = new Point[num_points];
	//Point* pc2  = new Point[num_points-1];
	//copy_array(pts, ppts, num_points);
	//copy_array(c2, pc2, num_points-1);
	//draw_bezier(ppts, num_points, c1, pc2, percent);
	//delete[] ppts;
	//delete[] pc2;
}
//Drawing circles
void draw_circle(int x, int y, int r)
{
	 draw_ellipse(x, y, r, r);
}
void draw_circle(Point center, int r)
{
	 draw_circle(center.x, center.y, r);
}
void draw_circle_filled(int x, int y, int r)
{
	 draw_ellipse_filled(x, y, r, r);
}
void draw_circle_filled(Point center, int r)
{
	 draw_circle_filled(center.x, center.y, r);
}
void draw_circle_3D(int x, int y, int r)
{
	 draw_ellipse_3D(x, y, r, r);
}
void draw_circle_3D(Point center, int r)
{
	 draw_circle_3D(center.x, center.y, r);
}
void draw_circle_3D_filled(int x, int y, int r)
{
	 draw_circle_3D(x, y, r);
	 draw_circle_filled(x, y, r);
}
void draw_circle_3D_filled(Point center, int r)
{
	 draw_circle_3D_filled(center.x, center.y, r);
}
void draw_circle_P3D(int x, int y, int r)
{
	 draw_ellipse_P3D(x, y, r, r);
}
void draw_circle_P3D(Point center, int r)
{
	 draw_circle_P3D(center.x, center.y, r);
}
void draw_circle_P3D_filled(int x, int y, int r)
{
	 draw_circle_P3D(x, y, r);
	 draw_circle_filled(x, y, r);
}
void draw_circle_P3D_filled(Point center, int r)
{
	 draw_circle_P3D_filled(center.x, center.y, r);
}
//Drawing ellipses
void draw_ellipse(Point left_top, Point right_bottom, int rot)
{
	if(rot == 0)
		draw_elliptic_arc(left_top, right_bottom, 0, 360);
	else
		class Ellipse(left_top, right_bottom, rot).draw();
}
void draw_ellipse(int x, int y, int rx, int ry, int rot)
{
	draw_ellipse(Point(x - rx, y - ry), Point(x + rx, y + ry), rot);
}
void draw_ellipse(Point center, int rx, int ry, int rot)
{
	draw_ellipse(center.x, center.y, rx, ry, rot);
}
void draw_ellipse_filled(Point left_top, Point right_bottom, int rot)
{
	if(rot == 0)
	{
		Ellipse(gfxe_hdc[gfx_active_page], 
			left_top.x, left_top.y, right_bottom.x + 1, right_bottom.y + 1);

		if(gfx_active_page == 0)
			Ellipse(gfxe_hdc[3], 
				left_top.x, left_top.y, right_bottom.x + 1, right_bottom.y + 1);
	}
	else
		class Ellipse(left_top, right_bottom, rot).draw_filled();

}
void draw_ellipse_filled(int x, int y, int rx, int ry, int rot)
{
	draw_ellipse_filled(Point(x - rx, y - ry), Point(x + rx, y + ry), rot);
}
void draw_ellipse_filled(Point center, int rx, int ry, int rot)
{
	draw_ellipse_filled(center.x, center.y, rx, ry, rot);
}
void draw_ellipse_3D(Point left_top, Point right_bottom, int rot)
{
	if(rot == 0)
		draw_elliptic_arc_3D(left_top, right_bottom, 0, 360);
	else
		class Ellipse(left_top, right_bottom, rot).draw_3D();
}
void draw_ellipse_3D(int x, int y, int rx, int ry, int rot)
{
	draw_ellipse_3D(Point(x - rx, y - ry), Point(x + rx, y + ry), rot);
}
void draw_ellipse_3D(Point center, int rx, int ry, int rot)
{
	draw_ellipse_3D(center.x, center.y, rx, ry, rot);
}
void draw_ellipse_3D_filled(Point left_top, Point right_bottom, int rot)
{
	draw_ellipse_3D(left_top, right_bottom, rot);
	draw_ellipse_filled(left_top, right_bottom, rot);
}
void draw_ellipse_3D_filled(int x, int y, int rx, int ry, int rot)
{
	draw_ellipse_3D(x, y, rx, ry, rot);
	draw_ellipse_filled(x, y, rx, ry, rot);
}
void draw_ellipse_3D_filled(Point center, int rx, int ry, int rot)
{
	draw_ellipse_3D_filled(center.x, center.y, rx, ry, rot);
}
void draw_ellipse_P3D(Point left_top, Point right_bottom, int rot)
{
	if(rot == 0)
		draw_elliptic_arc_P3D(left_top, right_bottom, 0, 360);
	else
		class Ellipse(left_top, right_bottom, rot).draw_P3D();
}
void draw_ellipse_P3D(int x, int y, int rx, int ry, int rot)
{
	draw_ellipse_P3D(Point(x - rx, y - ry), Point(x + rx, y + ry), rot);
}
void draw_ellipse_P3D(Point center, int rx, int ry, int rot)
{
	draw_ellipse_P3D(center.x, center.y, rx, ry, rot);
}
void draw_ellipse_P3D_filled(Point left_top, Point right_bottom, int rot)
{
	draw_ellipse_P3D(left_top, right_bottom, rot);
	draw_ellipse_filled(left_top, right_bottom, rot);
}
void draw_ellipse_P3D_filled(int x, int y, int rx, int ry, int rot)
{
	draw_ellipse_P3D(x, y, rx, ry, rot);
	draw_ellipse_filled(x, y, rx, ry, rot);
}
void draw_ellipse_P3D_filled(Point center, int rx, int ry, int rot)
{
	draw_ellipse_P3D_filled(center.x, center.y, rx, ry, rot);
}
//Drawing elliptic arcs
void draw_elliptic_arc(Point left_top, Point right_bottom, Point start_point, Point stop_point)
{
	Arc(gfxe_hdc[gfx_active_page], left_top.x, left_top.y, right_bottom.x, right_bottom.y, 
		start_point.x, start_point.y, stop_point.x, stop_point.y);

	if(gfx_active_page == 0)
		Arc(gfxe_hdc[3], left_top.x, left_top.y, right_bottom.x, right_bottom.y, 
		start_point.x, start_point.y, stop_point.x, stop_point.y);
}
void draw_elliptic_arc(int x, int y, int rx, int ry, Point start_point, Point stop_point)
{
	draw_elliptic_arc(Point(x - rx, y - ry), Point(x + rx, y + ry), start_point, stop_point);
}
void draw_elliptic_arc(Point center, int rx, int ry, Point start_point, Point stop_point)
{
	draw_elliptic_arc(center.x, center.y, rx, ry, start_point, stop_point);
}
void draw_elliptic_arc(Point left_top, Point right_bottom, int start_angle, int stop_angle)
{
	if(start_angle == stop_angle)
		return;
	int midx = (right_bottom.x + left_top.x) / 2;
	int midy = (right_bottom.y + left_top.y) / 2;
	Point start_point = circle_point(midx, midy, 400, start_angle);
	Point stop_point  = circle_point(midx, midy, 400, stop_angle);
	draw_elliptic_arc(left_top, right_bottom, start_point, stop_point);
}
void draw_elliptic_arc(int x, int y, int rx, int ry, int start_angle, int stop_angle)
{
	if(start_angle == stop_angle)
		return;
	Point start_point = circle_point(x, y, 400, start_angle);
	Point stop_point  = circle_point(x, y, 400, stop_angle);
	draw_elliptic_arc(Point(x - rx, y - ry), Point(x + rx, y + ry), start_point, stop_point);
}
void draw_elliptic_arc(Point center, int rx, int ry, int start_angle, int stop_angle)
{
	draw_elliptic_arc(center.x, center.y, rx, ry, start_angle, stop_angle);
}
void draw_elliptic_arc_filled(Point left_top, Point right_bottom, Point start_point, Point stop_point)
{
	Pie(gfxe_hdc[gfx_active_page], left_top.x, left_top.y, right_bottom.x, right_bottom.y, 
		start_point.x, start_point.y, stop_point.x, stop_point.y);

	if(gfx_active_page == 0)
		Pie(gfxe_hdc[3], left_top.x, left_top.y, right_bottom.x, right_bottom.y, 
		   start_point.x, start_point.y, stop_point.x, stop_point.y);
}
void draw_elliptic_arc_filled(int x, int y, int rx, int ry, Point start_point, Point stop_point)
{
	draw_elliptic_arc_filled(Point(x - rx, y - ry), Point(x + rx, y + ry), start_point, stop_point);
}
void draw_elliptic_arc_filled(Point center, int rx, int ry, Point start_point, Point stop_point)
{
	draw_elliptic_arc_filled(center.x, center.y, rx, ry, start_point, stop_point);
}
void draw_elliptic_arc_filled(Point left_top, Point right_bottom, int start_angle, int stop_angle)
{
	if(start_angle == stop_angle)
		return;
	int midx = (right_bottom.x + left_top.x) / 2;
	int midy = (right_bottom.y + left_top.y) / 2;
	Point start_point = circle_point(midx, midy, 400, start_angle);
	Point stop_point  = circle_point(midx, midy, 400, stop_angle);
	draw_elliptic_arc(left_top, right_bottom, start_point, stop_point);
}
void draw_elliptic_arc_filled(int x, int y, int rx, int ry, int start_angle, int stop_angle)
{
	if(start_angle == stop_angle)
		return;
	Point start_point = circle_point(x, y, 400, start_angle);
	Point stop_point  = circle_point(x, y, 400, stop_angle);
	draw_elliptic_arc_filled(Point(x - rx, y - ry), Point(x + rx, y + ry), start_point, stop_point);
}
void draw_elliptic_arc_filled(Point center, int rx, int ry, int start_angle, int stop_angle)
{
	draw_elliptic_arc_filled(center.x, center.y, rx, ry, start_angle, stop_angle);
}
void draw_elliptic_arc_3D(Point left_top, Point right_bottom, Point start_point, Point stop_point)
{
	int midx = (left_top.x + right_bottom.x) / 2;
	int midy = (left_top.y + right_bottom.y) / 2;
	int rx = (right_bottom.x - left_top.x) / 2;
	int ry = (right_bottom.y - left_top.y) / 2;
	draw_elliptic_arc_3D(midx, midy, rx, ry, start_point, stop_point);
}
void draw_elliptic_arc_3D(int x, int y, int rx, int ry, Point start_point, Point stop_point)
{
	Color save_color = gfx_pen_color;
	set_pen_color(gfx_3D_color);
	int depth;
	int new_x, new_y;
	int new_rx, new_ry;
	
	for(depth = 1; depth <= gfx_3D_depth; depth += gfx_3D_spacing)
	{
		new_rx = rx + depth * rx * gfx_3D_flare / (gfx_3D_depth * 45);
		new_ry = ry + depth * ry * gfx_3D_flare / (gfx_3D_depth * 45);
		new_x = x + (depth * gfx_3D_angle_x) / 45;
		new_y = y + (depth * gfx_3D_angle_y) / 45;
		draw_elliptic_arc(new_x, new_y, new_rx, new_ry, start_point, stop_point);
	}
	set_pen_color(save_color);
	draw_elliptic_arc(x, y, rx, ry, start_point, stop_point);
}
void draw_elliptic_arc_3D(Point center, int rx, int ry, Point start_point, Point stop_point)
{
	draw_elliptic_arc_3D(center.x, center.y, rx, ry, start_point, stop_point);
}
void draw_elliptic_arc_3D(Point left_top, Point right_bottom, int start_angle, int stop_angle)
{
	if(start_angle == stop_angle)
		return;
	int x = (left_top.x + right_bottom.x) / 2;
	int y = (left_top.y + right_bottom.y) / 2;
	int rx = (right_bottom.x - left_top.x) / 2;
	int ry = (right_bottom.y - left_top.y) / 2;
	draw_elliptic_arc_3D(x, y, rx, ry, ellipse_point(x, y, rx, ry, start_angle), ellipse_point(x, y, rx, ry, stop_angle));
}
void draw_elliptic_arc_3D(int x, int y, int rx, int ry, int start_angle, int stop_angle)
{
	if(start_angle == stop_angle)
		return;
	draw_elliptic_arc_3D(x, y, rx, ry, ellipse_point(x, y, rx, ry, start_angle), ellipse_point(x, y, rx, ry, stop_angle));
}
void draw_elliptic_arc_3D(Point center, int rx, int ry, int start_angle, int stop_angle)
{
	draw_elliptic_arc_3D(center.x, center.y, rx, ry, start_angle, stop_angle);
}
void draw_elliptic_arc_3D_filled(Point left_top, Point right_bottom, Point start_point, Point stop_point)
{
	draw_elliptic_arc_3D(left_top, right_bottom, start_point, stop_point);
	draw_elliptic_arc_filled(left_top, right_bottom, start_point, stop_point);
}
void draw_elliptic_arc_3D_filled(int x, int y, int rx, int ry, Point start_point, Point stop_point)
{
	draw_elliptic_arc_3D(x, y, rx, ry, start_point, stop_point);
	draw_elliptic_arc_filled(x, y, rx, ry, start_point, stop_point);
}
void draw_elliptic_arc_3D_filled(Point center, int rx, int ry, Point start_point, Point stop_point)
{
	draw_elliptic_arc_3D_filled(center.x, center.y, rx, ry, start_point, stop_point);
}
void draw_elliptic_arc_3D_filled(Point left_top, Point right_bottom, int start_angle, int stop_angle)
{
	draw_elliptic_arc_3D(left_top, right_bottom, start_angle, stop_angle);
	draw_elliptic_arc_filled(left_top, right_bottom, start_angle, stop_angle);
}
void draw_elliptic_arc_3D_filled(int x, int y, int rx, int ry, int start_angle, int stop_angle)
{
	draw_elliptic_arc_3D(x, y, rx, ry, start_angle, stop_angle);
	draw_elliptic_arc_filled(x, y, rx, ry, start_angle, stop_angle);
}
void draw_elliptic_arc_3D_filled(Point center, int rx, int ry, int start_angle,int stop_angle)
{
	draw_elliptic_arc_3D_filled(center.x, center.y, rx, ry, start_angle, stop_angle);
}
void draw_elliptic_arc_P3D(Point left_top, Point right_bottom, Point start_point, Point stop_point)
{
	int x = (left_top.x + right_bottom.x) / 2;
	int y = (left_top.y + right_bottom.y) / 2;
	int rx = (right_bottom.x - left_top.x) / 2;
	int ry = (right_bottom.y - left_top.y) / 2;
	draw_elliptic_arc_P3D(x, y, rx, ry, start_point, stop_point);
}
void draw_elliptic_arc_P3D(int x, int y, int rx, int ry, Point start_point, Point stop_point)
{
	//It doesn't work well to calculate the perspective shift on 
	//left_top and right_bottom individually and then draw
	//the resulting elliptic arc.  This version calculates the perspective 
	//shift from the center of the elliptic arc.
	Color save_color = gfx_pen_color;
	set_pen_color(gfx_3D_color);
	
	int depth;
	double dist_to_vp = distance(x, y, gfx_3D_vanishing_point);
	int max_depth = gfx_3D_depth;
	if( dist_to_vp < gfx_3D_depth)
		max_depth = int( dist_to_vp );
	
	Point new_center;
	int new_rx, new_ry;

	for(depth = 1; depth <= max_depth; depth += gfx_3D_spacing)
	{
		new_center = circle_point(x, y, depth, gfx_3D_vanishing_point);
		new_rx = int( double(rx) * distance(VanPt, new_center) / dist_to_vp);
		new_ry = int( double(ry) * distance(VanPt, new_center) / dist_to_vp);
		
		draw_elliptic_arc(new_center, new_rx, new_ry, start_point, stop_point);
	}
	set_pen_color(save_color);
	draw_elliptic_arc(x, y, rx, ry, start_point, stop_point);
}
void draw_elliptic_arc_P3D(Point center, int rx, int ry, Point start_point, Point stop_point)
{
	draw_elliptic_arc_P3D(center.x, center.y, rx, ry, start_point, stop_point);
}
void draw_elliptic_arc_P3D(Point left_top, Point right_bottom, int start_angle, int stop_angle)
{
	if(start_angle == stop_angle)
		return;
	int x = (left_top.x + right_bottom.x) / 2;
	int y = (left_top.y + right_bottom.y) / 2;
	int rx = (right_bottom.x - left_top.x) / 2;
	int ry = (right_bottom.y - left_top.y) / 2;
	draw_elliptic_arc_P3D(x, y, rx, ry, ellipse_point(x, y, rx, ry, start_angle), ellipse_point(x, y, rx, ry, stop_angle));
}
void draw_elliptic_arc_P3D(int x, int y, int rx, int ry, int start_angle, int stop_angle)
{
	if(start_angle == stop_angle)
		return;
	draw_elliptic_arc_P3D(x, y, rx, ry, ellipse_point(x, y, rx, ry, start_angle), ellipse_point(x, y, rx, ry, stop_angle));
}
void draw_elliptic_arc_P3D(Point center, int rx, int ry, int start_angle, int stop_angle)
{
	draw_elliptic_arc_P3D(center.x, center.y, rx, ry, start_angle, stop_angle);
}
void draw_elliptic_arc_P3D_filled(Point left_top, Point right_bottom, Point start_point, Point stop_point)
{
	draw_elliptic_arc_P3D(left_top, right_bottom, start_point, stop_point);
	draw_elliptic_arc_filled(left_top, right_bottom, start_point, stop_point);
}
void draw_elliptic_arc_P3D_filled(int x, int y, int rx, int ry, Point start_point, Point stop_point)
{
	draw_elliptic_arc_P3D(x, y, rx, ry, start_point, stop_point);
	draw_elliptic_arc_filled(x, y, rx, ry, start_point, stop_point);
}
void draw_elliptic_arc_P3D_filled(Point center, int rx, int ry, Point start_point, Point stop_point)
{
	draw_elliptic_arc_P3D_filled(center.x, center.y, rx, ry, start_point, stop_point);
}
void draw_elliptic_arc_P3D_filled(Point left_top,Point right_bottom, int start_angle, int stop_angle)
{
	draw_elliptic_arc_P3D(left_top, right_bottom, start_angle, stop_angle);
	draw_elliptic_arc_filled(left_top, right_bottom, start_angle, stop_angle);
}
void draw_elliptic_arc_P3D_filled(int x, int y, int rx, int ry, int start_angle, int stop_angle)
{
	draw_elliptic_arc_P3D(x, y, rx, ry, start_angle, stop_angle);
	draw_elliptic_arc_filled(x, y, rx, ry, start_angle, stop_angle);
}
void draw_elliptic_arc_P3D_filled(Point center, int rx, int ry, int start_angle, int stop_angle)
{
	draw_elliptic_arc_P3D_filled(center.x, center.y, rx, ry, start_angle, stop_angle);
}
//Drawing elliptic_ngons
void draw_elliptic_ngon(int x, int y, int rx, int ry, int n, int rot)
{
	Point* vertex_array = new Point[n];
	vertices_elliptic_ngon(vertex_array, x, y, rx, ry, n, rot);
	draw_polygon_f(vertex_array, n);
	delete[] vertex_array;
}
void draw_elliptic_ngon(Point center, int rx, int ry, int n, int rot)
{
	draw_elliptic_ngon(center.x, center.y, rx, ry, n, rot);
}
void draw_elliptic_ngon(Point left_top, Point right_bottom, int n, int rot)
{
	draw_elliptic_ngon((left_top.x + right_bottom.x) / 2, (left_top.y + right_bottom.y) / 2,
		               (right_bottom.x - left_top.x) / 2, (right_bottom.y - left_top.y) / 2, n, rot);
}
void draw_elliptic_ngon_filled(int x, int y, int rx, int ry, int n, int rot)
{
	Point* vertex_array = new Point[n];
	vertices_elliptic_ngon(vertex_array, x, y, rx, ry, n, rot);
	draw_polygon_filled_f(vertex_array, n);
	delete[] vertex_array;
}
void draw_elliptic_ngon_filled(Point center, int rx, int ry, int n, int rot)
{
	draw_elliptic_ngon_filled(center.x, center.y, rx, ry, n, rot);
}
void draw_elliptic_ngon_filled(Point left_top, Point right_bottom, int n, int rot)
{
	draw_elliptic_ngon_filled
		((left_top.x + right_bottom.x) / 2, (left_top.y + right_bottom.y) / 2,
		 (right_bottom.x - left_top.x) / 2, (right_bottom.y - left_top.y) / 2, n, rot);
}
void draw_elliptic_ngon_3D(int x, int y, int rx, int ry, int n, int rot)
{
	Color save_color = gfx_pen_color;
	set_pen_color(gfx_3D_color);
	int depth;
	int new_rx, new_ry;
	int new_x, new_y;
	int new_rot;
	for(depth = 1; depth <= gfx_3D_depth; depth += gfx_3D_spacing)
	{
		new_rx = rx + depth * rx * gfx_3D_flare / (gfx_3D_depth * 45);
		new_ry = ry + depth * ry * gfx_3D_flare / (gfx_3D_depth * 45);
		new_x = x + depth * gfx_3D_angle_x / 45;
		new_y = y + depth * gfx_3D_angle_y / 45;
		//Rotate 1 degree per pixel per 45 degrees of gfx_3D_twist.
		new_rot = rot + depth * gfx_3D_twist / 45;
		draw_elliptic_ngon(new_x, new_y, new_rx, new_ry, n, new_rot);
	}
	set_pen_color(save_color);
	draw_elliptic_ngon(x, y, rx, ry, n, rot);
}
void draw_elliptic_ngon_3D(Point center, int rx, int ry, int n, int rot)
{
	draw_elliptic_ngon_3D(center.x, center.y, rx, ry, n, rot);
}
void draw_elliptic_ngon_3D(Point left_top, Point right_bottom, int n, int rot)
{
	draw_elliptic_ngon_3D
		((left_top.x + right_bottom.x) / 2, (left_top.y + right_bottom.y) / 2,
		 (right_bottom.x - left_top.x) / 2, (right_bottom.y - left_top.y) / 2, n, rot);
}
void draw_elliptic_ngon_3D_filled(int x, int y, int rx, int ry, int n, int rot)
{
	draw_elliptic_ngon_3D(x, y, rx, ry, n, rot);
	draw_elliptic_ngon_filled(x, y, rx, ry, n, rot);
}
void draw_elliptic_ngon_3D_filled(Point center, int rx, int ry, int n, int rot)
{
	draw_elliptic_ngon_3D_filled(center.x, center.y, rx, ry, n, rot);
}
void draw_elliptic_ngon_3D_filled(Point left_top, Point right_bottom, int n, int rot)
{
	draw_elliptic_ngon_3D(left_top, right_bottom, n, rot);
	draw_elliptic_ngon_filled(left_top, right_bottom, n, rot);
}
void draw_elliptic_ngon_P3D(int x, int y, int rx, int ry, int n, int rot)
{
	//It doesn't work well to call draw_P3D_polygon which would draw each 
	//side individually.  Here we consider things from the center.
	Color save_color = gfx_pen_color;
	set_pen_color(gfx_3D_color);

	int depth, dist = int(distance(x, y, gfx_3D_vanishing_point));
	if(dist == 0)
		dist = 1;
	int max_depth = gfx_3D_depth;
	if( dist < max_depth)
		max_depth = dist;
	int new_rx, new_ry;
	Point new_center;
	int new_rot;

	for(depth = 1; depth <= max_depth; depth += gfx_3D_spacing)
	{
		new_center = circle_point(x, y, depth, gfx_3D_vanishing_point);
		new_rx = int(rx * distance(VanPt, new_center) / dist);
		new_ry = int(ry * distance(VanPt, new_center) / dist);
		new_rot = rot + depth * gfx_3D_twist / 45;
		draw_elliptic_ngon(new_center, new_rx, new_ry, n, new_rot);
	}
	set_pen_color(save_color);
	draw_elliptic_ngon(x, y, rx, ry, n, rot);
}
void draw_elliptic_ngon_P3D(Point center, int rx, int ry, int n, int rot)
{
	draw_elliptic_ngon_P3D(center.x, center.y, rx, ry, n, rot);
}
void draw_elliptic_ngon_P3D(Point left_top, Point right_bottom, int n, int rot)
{
	draw_elliptic_ngon_P3D
		((left_top.x + right_bottom.x) / 2, (left_top.y + right_bottom.y) / 2,
		 (right_bottom.x - left_top.x) / 2, (right_bottom.y - left_top.y) / 2, n, rot);
}
void draw_elliptic_ngon_P3D_filled(int x, int y, int rx, int ry, int n, int rot)
{
	draw_elliptic_ngon_P3D(x, y, rx, ry, n, rot);
	draw_elliptic_ngon_filled(x, y, rx, ry, n, rot);
}
void draw_elliptic_ngon_P3D_filled(Point center, int rx, int ry, int n, int rot)
{
	draw_elliptic_ngon_P3D_filled(center.x, center.y, rx, ry, n, rot);
}
void draw_elliptic_ngon_P3D_filled(Point left_top, Point right_bottom, int n, int rot)
{
	draw_elliptic_ngon_P3D(left_top, right_bottom, n, rot);
	draw_elliptic_ngon_filled(left_top, right_bottom, n, rot);
}
//Elliptic_outgons
void draw_elliptic_outgon(int x, int y, int rx, int ry, int n, int rot)
{
	Point* vertex_array = new Point[n];
	vertices_elliptic_outgon(vertex_array, x, y, rx, ry, n, rot);
	draw_polygon_f(vertex_array, n);
	delete[] vertex_array;
}
void draw_elliptic_outgon(Point center, int rx, int ry, int n, int rot)
{
	draw_elliptic_outgon(center.x, center.y, rx, ry, n, rot);
}
void draw_elliptic_outgon(Point left_top, Point right_bottom, int n, int rot)
{
	draw_elliptic_outgon
		((left_top.x + right_bottom.x) / 2, (left_top.y + right_bottom.y) / 2,
		 (right_bottom.x - left_top.x) / 2, (right_bottom.y - left_top.y) / 2, n, rot);
}
void draw_elliptic_outgon_filled(int x, int y, int rx, int ry, int n, int rot)
{
	Point* vertex_array = new Point[n];
	vertices_elliptic_outgon(vertex_array, x, y, rx, ry, n, rot);
	draw_polygon_filled_f(vertex_array, n);
	delete[] vertex_array;
}
void draw_elliptic_outgon_filled(Point center, int rx, int ry, int n, int rot)
{
	draw_elliptic_outgon_filled(center.x, center.y, rx, ry, n, rot);
}
void draw_elliptic_outgon_filled(Point left_top, Point right_bottom, int n, int rot)
{
	draw_elliptic_outgon_filled
		((left_top.x + right_bottom.x) / 2, (left_top.y + right_bottom.y) / 2,
		 (right_bottom.x - left_top.x) / 2, (right_bottom.y - left_top.y) / 2, n, rot);
}
void draw_elliptic_outgon_3D(int x, int y, int rx, int ry, int n, int rot)
{
	Color save_color = gfx_pen_color;
	set_pen_color(gfx_3D_color);
	int depth;
	int new_rx, new_ry;
	int new_x, new_y;
	int new_rot;
	for(depth = 1; depth <= gfx_3D_depth; depth += gfx_3D_spacing)
	{
		new_rx = rx + depth * rx * gfx_3D_flare / (gfx_3D_depth * 45);
		new_ry = ry + depth * ry * gfx_3D_flare / (gfx_3D_depth * 45);
		new_x = x + depth * gfx_3D_angle_x / 45;
		new_y = y + depth * gfx_3D_angle_y / 45;
		//Rotate 1 degree per pixel per 45 degrees of gfx_3D_twist.
		new_rot = rot + depth * gfx_3D_twist / 45;
		draw_elliptic_outgon(new_x, new_y, new_rx, new_ry, n, new_rot);
	}
	set_pen_color(save_color);
	draw_elliptic_outgon(x, y, rx, ry, n, rot);
}
void draw_elliptic_outgon_3D(Point center, int rx, int ry, int n, int rot)
{
	draw_elliptic_outgon_3D(center.x, center.y, rx, ry, n, rot);
}
void draw_elliptic_outgon_3D(Point left_top, Point right_bottom, int n, int rot)
{
	draw_elliptic_outgon_3D
		((left_top.x + right_bottom.x) / 2, (left_top.y + right_bottom.y) / 2,
		 (right_bottom.x - left_top.x) / 2, (right_bottom.y - left_top.y) / 2, n, rot);
}
void draw_elliptic_outgon_3D_filled(int x, int y, int rx, int ry, int n, int rot)
{
	draw_elliptic_outgon_3D(x, y, rx, ry, n, rot);
	draw_elliptic_outgon_filled(x, y, rx, ry, n, rot);
}
void draw_elliptic_outgon_3D_filled(Point center, int rx, int ry, int n, int rot)
{
	draw_elliptic_outgon_3D_filled(center.x, center.y, rx, ry, n, rot);
}
void draw_elliptic_outgon_3D_filled(Point left_top, Point right_bottom, int n, int rot)
{
	draw_elliptic_outgon_3D(left_top, right_bottom, n, rot);
	draw_elliptic_outgon_filled(left_top, right_bottom, n, rot);
}
void draw_elliptic_outgon_P3D(int x, int y, int rx, int ry, int n, int rot)
{
	//It doesn't work well to call draw_P3D_polygon which would draw each 
	//side individually.  Here we consider things from the center.
	Color save_color = gfx_pen_color;
	set_pen_color(gfx_3D_color);

	int depth, dist = int(distance(VanPt, Point(x,y)));
	if(dist == 0)
		dist = 1;
	int max_depth = gfx_3D_depth;
	if( dist < gfx_3D_depth)
		max_depth = dist;
	int new_rx, new_ry;
	Point new_center;
	int new_rot;

	for(depth = 1; depth <= max_depth; depth += gfx_3D_spacing)
	{
		new_center = circle_point(x, y, depth, gfx_3D_vanishing_point);
		new_rx = int(rx * distance(VanPt, new_center) / dist);
		new_ry = int(ry * distance(VanPt, new_center) / dist);
		new_rot = rot + depth * gfx_3D_twist / 45;
		draw_elliptic_outgon(new_center, new_rx, new_ry, n, new_rot);
	}
	set_pen_color(save_color);
	draw_elliptic_outgon(x, y, rx, ry, n, rot);
}
void draw_elliptic_outgon_P3D(Point center, int rx, int ry, int n, int rot)
{
	draw_elliptic_outgon_P3D(center.x, center.y, rx, ry, n, rot);
}
void draw_elliptic_outgon_P3D(Point left_top, Point right_bottom, int n, int rot)
{
	draw_elliptic_outgon_P3D
		((left_top.x + right_bottom.x) / 2, (left_top.y + right_bottom.y) / 2,
		 (right_bottom.x - left_top.x) / 2, (right_bottom.y - left_top.y) / 2, n, rot);
}
void draw_elliptic_outgon_P3D_filled(int x, int y, int rx, int ry, int n, int rot)
{
	draw_elliptic_outgon_P3D(x, y, rx, ry, n, rot);
	draw_elliptic_outgon_filled(x, y, rx, ry, n, rot);
}
void draw_elliptic_outgon_P3D_filled(Point center, int rx, int ry, int n, int rot)
{
	draw_elliptic_outgon_P3D_filled(center.x, center.y, rx, ry, n, rot);
}
void draw_elliptic_outgon_P3D_filled(Point left_top, Point right_bottom, int n, int rot)
{
	draw_elliptic_outgon_P3D(left_top, right_bottom, n, rot);
	draw_elliptic_outgon_filled(left_top, right_bottom, n, rot);
}
//Drawing input_text
std::string draw_input_text(int x, int y, std::string prompt)
{
	char ch = ' ';
	std::string input = "";
	Color save_text_color = get_text_color();
	draw_text(x, y, prompt + '|');		//Insert cursor
	while(true)
	{
		ch = get_char();
		if(ch == 13)
			break;
		if(ch == 27)
		{
			input += ch;
			break;
		}

		set_text_color(get_background_color());
		draw_text(x, y, prompt + input + '|');

		if((ch == 8))	//backspace
		{
			if(input.length() > 0) //else do nothing
				input = input.substr(0, input.length() - 1);
		}
		else
			input += ch;

		set_text_color(save_text_color);
		draw_text(x, y, prompt + input + '|');
	}
	set_text_color(get_background_color());
	draw_text(x, y, prompt + input + '|');
	set_text_color(save_text_color);
	draw_text(x, y, prompt + input);
	return input;
}
std::string draw_input_text(Point p, std::string prompt)
{
	return draw_input_text(p.x, p.y, prompt);
}
std::string draw_input_text_3D(int x, int y, std::string prompt)
{
	char ch = ' ';
	std::string input = "";
	Color save_text_color = get_text_color();
	Color save_3D_color = get_3D_color();
	draw_text_3D(x, y, prompt + '|');		//Insert cursor

	while(true)
	{
		ch = get_char();
		if(ch == 13)
			break;
		if(ch == 27)
		{
			input += ch;
			break;
		}

		set_text_color(get_background_color());
		set_3D_color(get_background_color());
		draw_text_3D(x, y, prompt + input + '|');

		if((ch == 8))	//backspace
		{
			if(input.length() > 0) //else do nothing
				input = input.substr(0, input.length() - 1);
		}
		else
			input += ch;

		set_text_color(save_text_color);
		set_3D_color(save_3D_color);
		draw_text_3D(x, y, prompt + input + '|');
	}
	set_text_color(get_background_color());
	set_3D_color(get_background_color());
	draw_text_3D(x, y, prompt + input + '|');
	set_text_color(save_text_color);
	set_3D_color(save_3D_color);
	draw_text_3D(x, y, prompt + input);
	return input;
}
std::string draw_input_text_3D(Point p, std::string prompt)
{
	return draw_input_text_3D(p.x, p.y, prompt);
}
std::string draw_input_text_P3D(int x, int y, std::string prompt)
{
	char ch = ' ';
	std::string input = "";
	Color save_text_color = get_text_color();
	Color save_3D_color = get_3D_color();
	draw_text_P3D(x, y, prompt + '|');		//Insert cursor

	while(true)
	{
		ch = get_char();
		if(ch == 13)
			break;
		if(ch == 27)
		{
			input += ch;
			break;
		}

		set_text_color(get_background_color());
		set_3D_color(get_background_color());
		draw_text_P3D(x, y, prompt + input + '|');

		if((ch == 8))	//backspace
		{
			if(input.length() > 0) //else do nothing
				input = input.substr(0, input.length() - 1);
		}
		else
			input += ch;

		set_text_color(save_text_color);
		set_3D_color(save_3D_color);
		draw_text_P3D(x, y, prompt + input + '|');
	}
	set_text_color(get_background_color());
	set_3D_color(get_background_color());
	draw_text_P3D(x, y, prompt + input + '|');
	set_text_color(save_text_color);
	set_3D_color(save_3D_color);
	draw_text_P3D(x, y, prompt + input);
	return input;
}
std::string draw_input_text_P3D(Point p, std::string prompt)
{
	return draw_input_text_P3D(p.x, p.y, prompt);
}
//Drawing lines
void draw_line(int x1, int y1, int x2, int y2)
{
	Point end_point[2] = {Point(x1, y1), Point(x2, y2)};
	draw_polyline_f(end_point, 2);
}
void draw_line(Point p, int x2, int y2)
{
	draw_line(p.x, p.y, x2, y2);
}
void draw_line(int x1, int y1, Point q)
{
	draw_line(x1, y1, q.x, q.y);
}
void draw_line(Point p, Point q)
{
	draw_line(p.x, p.y, q.x, q.y);
}
void draw_line_3D(int x1, int y1, int x2, int y2)
{
	Point end_point[2] = {Point(x1, y1), Point(x2, y2)};
	draw_polyline_3D_f(end_point, 2);	
}
void draw_line_3D(Point p, int x2, int y2)
{
	draw_line_3D(p.x, p.y, x2, y2);
}
void draw_line_3D(int x1, int y1, Point q)
{
	draw_line_3D(x1, y1, q.x, q.y);
}
void draw_line_3D(Point p, Point q)
{
	draw_line_3D(p.x, p.y, q.x, q.y);
}
void draw_line_P3D(int x1, int y1, int x2, int y2)
{
	Point end_point[] = {Point(x1,y1), Point(x2,y2)};
	draw_polyline_P3D_f(end_point, 2);
}
void draw_line_P3D(Point p, int x2, int y2)
{
	draw_line_P3D(p.x, p.y, x2, y2);
}
void draw_line_P3D(int x1, int y1, Point q)
{
	draw_line_P3D(x1, y1, q.x, q.y);
}
void draw_line_P3D(Point p, Point q)
{
	draw_line_P3D(p.x, p.y, q.x, q.y);
}
//Drawing ngons
void draw_ngon(int x, int y, int r, int n, int rot)
{
	draw_elliptic_ngon(x, y, r, r, n, rot);
}
void draw_ngon(Point center, int r, int n, int rot)
{
	draw_ngon(center.x, center.y, r, n, rot);
}
void draw_ngon_filled(int x, int y, int r, int n, int rot)
{
	draw_elliptic_ngon_filled(x, y, r, r, n, rot);
}
void draw_ngon_filled(Point center, int r, int n, int rot)
{
	draw_ngon_filled(center.x, center.y, r, n, rot);
}
void draw_ngon_3D(int x, int y, int r, int n, int rot)
{
	draw_elliptic_ngon_3D(x, y, r, r, n, rot);
}
void draw_ngon_3D(Point center, int r, int n, int rot)
{
	draw_ngon_3D(center.x, center.y, r, n, rot);
}
void draw_ngon_3D_filled(int x, int y, int r, int n, int rot)
{
	draw_ngon_3D(x, y, r, n, rot);
	draw_ngon_filled(x, y, r, n, rot);
}
void draw_ngon_3D_filled(Point center, int r, int n, int rot)
{
	draw_ngon_3D_filled(center.x, center.y, r, n, rot);
}
void draw_ngon_P3D(int x, int y, int r, int n, int rot)
{
	draw_elliptic_ngon_P3D(x, y, r, r, n, rot);
}
void draw_ngon_P3D(Point center, int r, int n, int rot)
{
	draw_ngon_P3D(center.x, center.y, r, n, rot);
}
void draw_ngon_P3D_filled(int x, int y, int r, int n, int rot)
{
	draw_ngon_P3D(x, y, r, n, rot);
	draw_ngon_filled(x, y, r, n, rot);
}
void draw_ngon_P3D_filled(Point center, int r, int n, int rot)
{
	draw_ngon_P3D_filled(center.x, center.y, r, n, rot);
}
//Drawing outgons
void draw_outgon(int x, int y, int r, int n, int rot)
{
	draw_elliptic_outgon(x, y, r, r, n, rot);
}
void draw_outgon(Point center, int r, int n, int rot)
{
	draw_outgon(center.x, center.y, r, n, rot);
}
void draw_outgon_filled(int x, int y, int r, int n, int rot)
{
	draw_elliptic_outgon_filled(x, y, r, r, n, rot);
}
void draw_outgon_filled(Point center, int r, int n, int rot)
{
	draw_outgon_filled(center.x, center.y, r, n, rot);
}
void draw_outgon_3D(int x, int y, int r, int n, int rot)
{
	draw_elliptic_outgon_3D(x, y, r, r, n, rot);
}
void draw_outgon_3D(Point center, int r, int n, int rot)
{
	draw_outgon_3D(center.x, center.y, r, n, rot);
}
void draw_outgon_3D_filled(int x, int y, int r, int n, int rot)
{
	draw_outgon_3D(x, y, r, n, rot);
	draw_outgon_filled(x, y, r, n, rot);
}
void draw_outgon_3D_filled(Point center, int r, int n, int rot)
{
	draw_outgon_3D_filled(center.x, center.y, r, n, rot);
}
void draw_outgon_P3D(int x, int y, int r, int n, int rot)
{
	draw_elliptic_outgon_P3D(x, y, r, r,  n, rot);
}
void draw_outgon_P3D(Point center, int r, int n, int rot)
{
	draw_outgon_P3D(center.x, center.y, r, n, rot);
}
void draw_outgon_P3D_filled(int x, int y, int r, int n, int rot)
{
	draw_outgon_P3D(x, y, r, n, rot);
	draw_outgon_filled(x, y, r, n, rot);
}
void draw_outgon_P3D_filled(Point center, int r, int n, int rot)
{
	draw_outgon_P3D_filled(center.x, center.y, r, n, rot);
}
//Drawing points
void draw_point(int x, int y)
{
	SetPixel(gfxe_hdc[gfx_active_page], x, y, RGB(gfx_pixel_color.r,gfx_pixel_color.g,gfx_pixel_color.b));
	if(gfx_active_page == 0)
		SetPixel(gfxe_hdc[3], x, y, RGB(gfx_pixel_color.r,gfx_pixel_color.g,gfx_pixel_color.b));
}
void draw_point(Point center)
{
	draw_point(center.x, center.y);
}
void draw_point_3D(int x, int y)
{
	int depth;
	Color save_color = gfx_pixel_color;
	set_pen_color(gfx_3D_color);

	for(depth = 1; depth <= gfx_3D_depth; depth += gfx_3D_spacing )
		draw_point(x + (depth * gfx_3D_angle_x) / 45, 
				   y + (depth * gfx_3D_angle_y) / 45);

	gfx_pixel_color = save_color;
	draw_point(x, y);
}
void draw_point_3D(Point center)
{
	draw_point_3D(center.x, center.y);
}
void draw_point_P3D(int x, int y)
{
	Color save_color = gfx_pixel_color;
	set_pen_color(gfx_3D_color);
	
	int depth, dist_to_vp = int( distance( x, y, gfx_3D_vanishing_point ) );
	int max_depth = gfx_3D_depth;
	if( dist_to_vp < gfx_3D_depth )
		max_depth = dist_to_vp;
	for( depth = 1; depth <= max_depth; depth++ )
	{
		draw_point(circle_point(x, y, depth, gfx_3D_vanishing_point));
	}
	gfx_pixel_color = save_color;
	draw_point(x,y);
}
void draw_point_P3D(Point center)
{
	draw_point_P3D(center.x, center.y);
}
//Drawing polygons
//polygons_f - These functions are called by the draw_polygon macros.
void draw_polygon_f(Point* vertex_array, int ns)
{
	draw_polyline_f(vertex_array, ns);
	draw_line(vertex_array[0], vertex_array[ns - 1]);
}
void draw_polygon_f(int* coord_array, int ns)
{
	draw_polyline_f(coord_array, ns);
	draw_line(coord_array[0], coord_array[1], coord_array[2*ns - 2], coord_array[2*ns - 1]);
}
void draw_polygon_f(VariablePoint* vertex_array, int ns)
{
	draw_polyline_f(vertex_array, ns);
	draw_line(vertex_array[0], vertex_array[ns - 1]);
}
void draw_polygon_filled_f(Point* vertex_array, int ns)
{
	POINT* p = new POINT[ns];	//POINT x & y fields are long
	int c;
	for(c = 0; c < ns; c++)
	{
		p[c].x = long(vertex_array[c].x);
		p[c].y = long(vertex_array[c].y);
	}

	Polygon(gfxe_hdc[gfx_active_page], p, ns);
	if(gfx_active_page == 0)
		Polygon(gfxe_hdc[3], p, ns);

	delete[] p;
}
void draw_polygon_filled_f(int* coord_array, int ns)
{
	POINT* p = new POINT[ns];	//POINT x & y fields are long
	int c;
	for(c = 0; c < ns; c++)
	{
		p[c].x = long(coord_array[2*c]);
		p[c].y = long(coord_array[2*c + 1]);
	}

	Polygon(gfxe_hdc[gfx_active_page], p, ns);
	if(gfx_active_page == 0)
		Polygon(gfxe_hdc[3], p, ns);

	delete[] p;
}
void draw_polygon_filled_f(VariablePoint* vertex_array, int ns)
{
	POINT* p = new POINT[ns];	//POINT x & y fields are long
	int c;
	for(c = 0; c < ns; c++)
	{
		p[c].x = long(Point(vertex_array[c]).x);
		p[c].y = long(Point(vertex_array[c]).y);
	}

	Polygon(gfxe_hdc[gfx_active_page], p, ns);
	if(gfx_active_page == 0)
		Polygon(gfxe_hdc[3], p, ns);

	delete[] p;
}
void draw_polygon_3D_f(Point* vertex_array, int ns)
{
	draw_polyline_3D_f(vertex_array, ns);
	draw_line_3D(vertex_array[0], vertex_array[ns - 1]);
}
void draw_polygon_3D_f(int* coord_array, int ns)
{
	draw_polyline_3D_f(coord_array, ns);
	draw_line_3D(coord_array[0], coord_array[1], coord_array[2*ns - 2], coord_array[2*ns - 1]);
}
void draw_polygon_3D_f(VariablePoint* vertex_array, int ns)
{
	draw_polyline_3D_f(vertex_array, ns);
	draw_line_3D(vertex_array[0], vertex_array[ns - 1]);
}
void draw_polygon_3D_filled_f(Point* vertex_array, int ns)
{
	draw_polygon_3D_f(vertex_array, ns);
	draw_polygon_filled_f(vertex_array, ns);
}
void draw_polygon_3D_filled_f(int* coord_array, int ns)
{
	draw_polygon_3D_f(coord_array, ns);
	draw_polygon_filled_f(coord_array, ns);
}
void draw_polygon_3D_filled_f(VariablePoint* vertex_array, int ns)
{
	draw_polygon_3D_f(vertex_array, ns);
	draw_polygon_filled_f(vertex_array, ns);
}
void draw_polygon_P3D_f(Point* vertex_array, int ns)
{
	draw_polyline_P3D_f(vertex_array, ns);
	draw_line_P3D(vertex_array[0], vertex_array[ns - 1]);
}
void draw_polygon_P3D_f(int* coord_array, int ns)
{
	draw_polyline_P3D_f(coord_array, ns);
	draw_line_P3D(coord_array[0], coord_array[1], coord_array[2*ns - 2], coord_array[2*ns - 1]);
}
void draw_polygon_P3D_f(VariablePoint* vertex_array, int ns)
{
	draw_polyline_P3D_f(vertex_array, ns);
	draw_line_P3D(vertex_array[0], vertex_array[ns - 1]);
}
void draw_polygon_P3D_filled_f(Point* vertex_array, int ns)
{
	draw_polygon_P3D_f(vertex_array, ns);
	draw_polygon_filled_f(vertex_array, ns);
}
void draw_polygon_P3D_filled_f(int* coord_array, int ns)
{
	draw_polygon_P3D_f(coord_array, ns);
	draw_polygon_filled_f(coord_array, ns);
}
void draw_polygon_P3D_filled_f(VariablePoint* vertex_array, int ns)
{
	draw_polygon_P3D_f(vertex_array, ns);
	draw_polygon_filled_f(vertex_array, ns);
}
//Drawing polylines
//polylines_f - These functions are called by the draw_polyline macros.
void draw_polyline_f(Point* vertex_array, int num_points)
{
	POINT* p = new POINT[num_points]; //POINT x & y fields are long
	int c;
	for(c = 0; c < num_points; c++)
	{
		p[c].x = long(vertex_array[c].x);
		p[c].y = long(vertex_array[c].y);
	}

	Polyline(gfxe_hdc[gfx_active_page], p, num_points);
	//The Polyline function does not draw the final point -- note it should be in pen_color!
	SetPixel(gfxe_hdc[gfx_active_page],p[num_points-1].x,p[num_points-1].y,RGB(gfx_pen_color.r, gfx_pen_color.g, gfx_pen_color.b));

	if(gfx_active_page == 0)
	{
		Polyline(gfxe_hdc[3], p, num_points);
		SetPixel(gfxe_hdc[gfx_active_page],p[num_points-1].x,p[num_points-1].y,RGB(gfx_pen_color.r, gfx_pen_color.g, gfx_pen_color.b));
	}
	delete[] p;
}
void draw_polyline_f(int* coord_array, int num_points)
{
	POINT* p = new POINT[num_points]; //POINT x & y fields are long
	int c;
	for(c = 0; c < num_points; c++)
	{
		p[c].x = long(coord_array[2*c]);
		p[c].y = long(coord_array[2*c+1]);
	}

	Polyline(gfxe_hdc[gfx_active_page], p, num_points);
	//The Polyline function does not draw the final point -- note it should be in pen_color!
	SetPixel(gfxe_hdc[gfx_active_page],p[num_points-1].x,p[num_points-1].y,RGB(gfx_pen_color.r, gfx_pen_color.g, gfx_pen_color.b));

	if(gfx_active_page == 0)
	{
		Polyline(gfxe_hdc[3], p, num_points);
		SetPixel(gfxe_hdc[gfx_active_page], p[num_points-1].x, p[num_points-1].y, RGB(gfx_pen_color.r, gfx_pen_color.g, gfx_pen_color.b));
	}
	delete[] p;
}
void draw_polyline_f(VariablePoint* vertex_array, int num_points)
{
	POINT* p = new POINT[num_points]; //POINT x & y fields are long
	int c;
	for(c = 0; c < num_points; c++)
	{
		p[c].x = long(Point(vertex_array[c]).x);
		p[c].y = long(Point(vertex_array[c]).y);
	}

	Polyline(gfxe_hdc[gfx_active_page], p, num_points);
	//The Polyline function does not draw the final point -- note it should be in pen_color!
	SetPixel(gfxe_hdc[gfx_active_page],p[num_points-1].x,p[num_points-1].y,RGB(gfx_pen_color.r, gfx_pen_color.g, gfx_pen_color.b));

	if(gfx_active_page == 0)
	{
		Polyline(gfxe_hdc[3], p, num_points);
		SetPixel(gfxe_hdc[gfx_active_page],p[num_points-1].x,p[num_points-1].y,RGB(gfx_pen_color.r, gfx_pen_color.g, gfx_pen_color.b));
	}
	delete[] p;
}
void draw_polyline_3D_f(Point* vertex_array, int num_points)
{
	Color save_color = gfx_pen_color;
	set_pen_color(gfx_3D_color);
	Point* new_verts = new Point[num_points];

	int depth, i;
	
	for(depth = 1; depth <= gfx_3D_depth; depth += gfx_3D_spacing)
	{
		for(i = 0; i < num_points; i++)
		{
			new_verts[i].x = vertex_array[i].x  + depth * gfx_3D_angle_x / 45;
			new_verts[i].y = vertex_array[i].y  + depth * gfx_3D_angle_y / 45;
		}
		draw_polyline_f(new_verts, num_points);
	}
	set_pen_color(save_color);
	draw_polyline_f(vertex_array, num_points);
	delete[] new_verts;
}
void draw_polyline_3D_f(int* coord_array, int num_points)
{
	Color save_color = gfx_pen_color;
	set_pen_color(gfx_3D_color);
	Point* new_verts = new Point[num_points];

	int depth, i;
	
	for(depth = 1; depth <= gfx_3D_depth; depth += gfx_3D_spacing)
	{
		for(i = 0; i < num_points; i++)
		{
			new_verts[i].x = coord_array[2 * i] + depth * gfx_3D_angle_x / 45;
			new_verts[i].y = coord_array[2 * i + 1] + depth * gfx_3D_angle_y / 45;
		}
		draw_polyline_f(new_verts, num_points);
	}
	set_pen_color(save_color);
	draw_polyline_f(coord_array, num_points);
	delete[] new_verts;
}
void draw_polyline_3D_f(VariablePoint* vertex_array, int num_points)
{
	Color save_color = gfx_pen_color;
	set_pen_color(gfx_3D_color);
	Point* new_verts = new Point[num_points];

	int depth, i;
	
	for(depth = 1; depth <= gfx_3D_depth; depth += gfx_3D_spacing)
	{
		for(i = 0; i < num_points; i++)
		{
			new_verts[i].x = Point(vertex_array[i]).x  + depth * gfx_3D_angle_x / 45;
			new_verts[i].y = Point(vertex_array[i]).y  + depth * gfx_3D_angle_y / 45;
		}
		draw_polyline_f(new_verts, num_points);
	}
	set_pen_color(save_color);
	draw_polyline_f(vertex_array, num_points);
	delete[] new_verts;
}
void draw_polyline_P3D_f(Point* vertex_array, int num_points)
{
	/*The output of this function is slightly "warped".  The reason is that 
	we are scaling the points back toward the vanishing point by an
	absolute number of pixels rather than by a percentage.  It shouldn't 
	be too hard to fix it.  I'd find the center of the convex hull of 
	the polyline points and use it as my reference...
	But I do like "warped" effect and would like to be able to have that
	rendering as well. (P3D_warped?)
  */
	Color save_color = gfx_pen_color;
	set_pen_color(gfx_3D_color);
	
	Point* pts = new Point[num_points];
	int i;
	for(i = 0; i < num_points; i++)
		pts[i] = vertex_array[i];
	int depth;
	for(depth = 1; depth<= gfx_3D_depth; depth += gfx_3D_spacing)
	{
		for(i = 0; i < num_points; i++)
			pts[i] = circle_point(vertex_array[i], depth, gfx_3D_vanishing_point);
		draw_polyline_f(pts, num_points);
	}
	set_pen_color(save_color);
	draw_polyline_f(vertex_array, num_points);
	delete[] pts;
}
void draw_polyline_P3D_f(int* coord_array, int num_points)
{
	Color save_color = gfx_pen_color;
	set_pen_color(gfx_3D_color);
	
	Point* pts = new Point[num_points];
	int i;
	for(i = 0; i < num_points; i++)
	{
		pts[i].x = coord_array[2*i];
		pts[i].y = coord_array[2*i+1];
	}
	int depth;
	for(depth = 1; depth<= gfx_3D_depth; depth += gfx_3D_spacing)
	{
		for(i = 0; i < num_points; i++)
			pts[i] = circle_point(coord_array[2*i], coord_array[2*i+1], depth, gfx_3D_vanishing_point);
		draw_polyline_f(pts, num_points);
	}
	set_pen_color(save_color);
	draw_polyline_f(coord_array, num_points);
	delete[] pts;
}
void draw_polyline_P3D_f(VariablePoint* vertex_array, int num_points)
{
	/*The output of this function is slightly "warped".  The reason is that 
	we are scaling the points back toward the vanishing point by an
	absolute number of pixels rather than by a percentage.  It shouldn't 
	be too hard to fix it.  I'd find the center of the convex hull of 
	the polyline points and use it as my reference...
	But I do like "warped" effect and would like to be able to have that
	rendering as well. (P3D_warped?)
  */
	Color save_color = gfx_pen_color;
	set_pen_color(gfx_3D_color);
	
	Point* pts = new Point[num_points];
	int i;
	for(i = 0; i < num_points; i++)
		pts[i] = vertex_array[i];
	int depth;
	for(depth = 1; depth<= gfx_3D_depth; depth += gfx_3D_spacing)
	{
		for(i = 0; i < num_points; i++)
			pts[i] = circle_point(vertex_array[i], depth, gfx_3D_vanishing_point);
		draw_polyline_f(pts, num_points);
	}
	set_pen_color(save_color);
	draw_polyline_f(vertex_array, num_points);
	delete[] pts;
}
//Drawing rectangles
void draw_rectangle(Point left_top, Point right_bottom, int rot)
{
	if(rot == 0)
	{
		POINT rect[5];	//POINT x & y fields are long
		rect[0].x = long(left_top.x);			rect[0].y = long(left_top.y);
		rect[1].x = long(right_bottom.x);		rect[1].y = long(left_top.y);
		rect[2].x = long(right_bottom.x);		rect[2].y = long(right_bottom.y);
		rect[3].x = long(left_top.x);			rect[3].y = long(right_bottom.y);
		rect[4] = rect[0];

		Polyline(gfxe_hdc[gfx_active_page], rect, 5);
		if(gfx_active_page == 0)
			Polyline(gfxe_hdc[3], rect, 5);
	}
	else
		draw_elliptic_outgon((left_top.x + right_bottom.x)/2,
		            (left_top.y + right_bottom.y)/2,
					(right_bottom.x - left_top.x)/2,
					(right_bottom.y - left_top.y)/2, 4, rot);
}
void draw_rectangle(int x, int y, int rx, int ry, int rot)
{
		draw_rectangle(Point(x - rx, y - ry), Point(x + rx, y + ry), rot);
}
void draw_rectangle(Point center, int rx, int ry, int rot)
{
	draw_rectangle(center.x, center.y, rx, ry, rot);
}
void draw_rectangle_filled(Point left_top, Point right_bottom, int rot)
{
	if(rot == 0)
	{
		Rectangle(gfxe_hdc[gfx_active_page], 
			left_top.x, left_top.y, right_bottom.x+1, right_bottom.y + 1);

		if(gfx_active_page == 0)
			Rectangle(gfxe_hdc[3], 
				left_top.x, left_top.y, right_bottom.x+1, right_bottom.y + 1);
	}
	else
		draw_elliptic_outgon_filled(
					(left_top.x + right_bottom.x)/2,
		            (left_top.y + right_bottom.y)/2,
					(right_bottom.x - left_top.x)/2,
					(right_bottom.y - left_top.y)/2, 4, rot);
}
void draw_rectangle_filled(int x, int y, int rx, int ry, int rot)
{
	draw_rectangle_filled(Point(x - rx, y - ry), Point(x + rx, y + ry), rot);
}
void draw_rectangle_filled(Point center, int rx, int ry, int rot)
{
	draw_rectangle_filled(center.x, center.y, rx, ry, rot);
}
void draw_rectangle_3D(Point left_top, Point right_bottom, int rot)
{
	int x = (left_top.x + right_bottom.x) / 2;
	int y = (left_top.y + right_bottom.y) / 2;
	int rx = (right_bottom.x - left_top.x) / 2;
	int ry = (right_bottom.y - left_top.y) / 2;
	draw_rectangle_3D(x, y, rx, ry);
}
void draw_rectangle_3D(int x, int y, int rx, int ry, int rot)
{
	Color save_color = gfx_pen_color;
	set_pen_color(gfx_3D_color);
	int depth;
	int new_rx, new_ry;
	int new_x, new_y;
	for(depth = 1; depth <= gfx_3D_depth; depth += gfx_3D_spacing)
	{
		new_rx = rx + depth * rx * gfx_3D_flare / (gfx_3D_depth * 45);
		new_ry = ry + depth * ry * gfx_3D_flare / (gfx_3D_depth * 45);
		new_x = x + depth * gfx_3D_angle_x / 45;
		new_y = y + depth * gfx_3D_angle_y / 45;
		draw_rectangle(new_x, new_y, new_rx, new_ry, rot + depth * gfx_3D_twist / 45);
	}
	set_pen_color(save_color);
	draw_rectangle(x, y, rx, ry, rot);
}
void draw_rectangle_3D(Point center, int rx, int ry, int rot)
{
	draw_rectangle_3D(center.x, center.y, rx, ry, rot);
}
void draw_rectangle_3D_filled(Point left_top, Point right_bottom, int rot)
{
	draw_rectangle_3D(left_top, right_bottom, rot);
	draw_rectangle_filled(left_top, right_bottom, rot);
}
void draw_rectangle_3D_filled(int x, int y, int rx, int ry, int rot)
{
	draw_rectangle_3D(x, y, rx, ry, rot);
	draw_rectangle_filled(x, y, rx, ry, rot);
}
void draw_rectangle_3D_filled(Point center, int rx, int ry, int rot)
{
	draw_rectangle_3D_filled(center.x, center.y, rx, ry, rot);
}
void draw_rectangle_P3D(Point left_top, Point right_bottom, int rot)
{
	int x = (left_top.x + right_bottom.x) / 2;
	int y = (left_top.y + right_bottom.y) / 2;
	int rx = (right_bottom.x - left_top.x) / 2;
	int ry = (right_bottom.y - left_top.y) / 2;
	draw_rectangle_P3D(x, y, rx, ry, rot);
}
void draw_rectangle_P3D(int x, int y, int rx, int ry, int rot)
{
	//It doesn't work well to calculate the perspective shift on 
	//left_top and right_bottom individually and then draw
	//the resulting rectangle.  This version calculates the perspective 
	//shift from the center of the rectangle.
	Color save_color = gfx_pen_color;
	set_pen_color(gfx_3D_color);
	
	int depth, min, dist = int(distance(x, y, VanPt));
	if( (min = gfx_3D_depth) > dist)
		min = dist;
	int new_rx, new_ry, new_rot;
	Point new_center;

	for(depth = 1; depth <= min; depth += gfx_3D_spacing)
	{
		new_center = circle_point(x, y, depth, gfx_3D_vanishing_point);
		new_rx = int(rx * distance(VanPt, new_center) / dist);
		new_ry = int(ry * distance(VanPt, new_center) / dist);
		new_rot = rot + depth * gfx_3D_twist / 45;
		draw_rectangle(new_center.x, new_center.y, new_rx, new_ry, new_rot);
	}
	set_pen_color(save_color);
	draw_rectangle(x, y, rx, ry, rot);
}
void draw_rectangle_P3D(Point center, int rx, int ry, int rot)
{
	draw_rectangle_P3D(center.x, center.y, rx, ry, rot);
}
void draw_rectangle_P3D_filled(Point left_top, Point right_bottom, int rot)
{
	draw_rectangle_P3D(left_top, right_bottom, rot);
	draw_rectangle_filled(left_top, right_bottom, rot);
}
void draw_rectangle_P3D_filled(int x, int y, int rx, int ry, int rot)
{
	draw_rectangle_P3D(x, y, rx, ry, rot);
	draw_rectangle_filled(x, y, rx, ry, rot);
}
void draw_rectangle_P3D_filled(Point center, int rx, int ry, int rot)
{
	draw_rectangle_P3D_filled(center.x, center.y, rx, ry, rot);
}

//Rounded Rectangles are used by the draw_button package
void draw_rounded_rectangle(Point left_top, Point right_bottom, 
								   int widthPercent, int heightPercent)
{
	if(widthPercent < 0) widthPercent = 0;
	if(heightPercent < 0) heightPercent = 0;
	RoundRect(gfxe_hdc[gfx_active_page], 
		left_top.x, left_top.y, right_bottom.x+1, right_bottom.y+1,
		(right_bottom.x - left_top.x) * widthPercent / 100,
		(right_bottom.y - left_top.y) * heightPercent / 100);
	
	if(gfx_active_page == 0)
		RoundRect(gfxe_hdc[3], 
		   left_top.x, left_top.y, right_bottom.x+1, right_bottom.y+1,
		   (right_bottom.x - left_top.x) * widthPercent / 100,
		   (right_bottom.y - left_top.y) * heightPercent / 100);
}

////Drawing squares
void draw_square(int x, int y, int r, int rot)
{
	draw_rectangle(x, y, r, r, rot);
}
void draw_square(Point center, int r, int rot)
{
	draw_square(center.x, center.y, r, rot);
}
void draw_square_filled(int x, int y, int r, int rot)
{
	draw_rectangle_filled(x, y, r, r, rot);
}
void draw_square_filled(Point center, int r, int rot)
{
	draw_square_filled(center.x, center.y, r, rot);
}
void draw_square_3D(int x, int y, int r, int rot)
{
	draw_rectangle_3D(x, y, r, r, rot);
}
void draw_square_3D(Point center, int r, int rot)
{
	draw_square_3D(center.x, center.y, r, rot);
}
void draw_square_3D_filled(int x, int y, int r, int rot)
{
	draw_square_3D(x, y, r, rot);
	draw_square_filled(x, y, r, rot);
}
void draw_square_3D_filled(Point center, int r, int rot)
{
	draw_square_3D_filled(center.x, center.y, r, rot);
}
void draw_square_P3D(int x, int y, int r, int rot)
{
	draw_rectangle_P3D(x, y, r, r, rot);
}
void draw_square_P3D(Point center, int r, int rot)
{
	draw_square_P3D(center.x, center.y, r, rot);
}
void draw_square_P3D_filled(int x, int y, int r, int rot)
{
	draw_square_P3D(x, y, r, rot);
	draw_square_filled(x, y, r, rot);
}
void draw_square_P3D_filled(Point center, int r, int rot)
{
	draw_square_P3D_filled(center.x, center.y, r, rot);
}

//Text
void draw_text(int x, int y, std::string message)
{
	if(gfx_text_alignment == CENTER)
	{
		y = int(y + text_height(message) * cos(gfx_text_rotation * DEG2RAD) / 4.0);
		x = int(x + text_height(message) * sin(gfx_text_rotation * DEG2RAD) / 2.0);
	}
	//With Express 9, webdings over 126 were not printing.  The fix was to go to
	//Project->Props->Config->Gen->CharSet
	//and remove Unicode.
#ifdef _UNICODE
	//The following is needed because the project default is Unicode:
	//Project -> Properties -> Configuration -> General -> Character Set

	TCHAR* unicodePtr = new TCHAR[message.length() + 1];
	for(int i = 0; i < (int) message.length(); i++)
		unicodePtr[i] = message[i];
	unicodePtr[message.length()] = '\0';

    TextOut(gfxe_hdc[gfx_active_page], x, y, unicodePtr, (int)message.length());
	if(gfx_active_page == 0)
		TextOut(gfxe_hdc[3], x, y, unicodePtr, (int)message.length());

	delete[] unicodePtr;
#else
    TextOut(gfxe_hdc[gfx_active_page], x, y, (LPCSTR) message.c_str(), (int)message.length());
	if(gfx_active_page == 0)
		TextOut(gfxe_hdc[3], x, y, (LPCSTR) message.c_str(), (int)message.length());
#endif
}
void draw_text(Point p, std::string message)
{
	draw_text(p.x, p.y, message);
}
void draw_text(int x, int y, char ch)
{
	draw_text(x, y, to_string(ch));
}
void draw_text(Point p, char ch)
{
	draw_text(p.x, p.y, to_string(ch));
}
void draw_text(int x, int y, int n)
{
	draw_text(x, y, to_string(n));
}
void draw_text(Point p, int n)
{
	draw_text(p.x, p.y, n);
}
void draw_text(int x, int y, unsigned int n)
{
	draw_text(x, y, to_string(n));
}
void draw_text(Point p, unsigned int n)
{
	draw_text(p.x, p.y, n);
}
void draw_text(int x, int y, double n)
{
	draw_text(x, y, to_string(n));
}
void draw_text(Point p, double n)
{
	draw_text(p.x, p.y, n);
}
void draw_text_3D(int x, int y, std::string message)
{
	Color old_color = get_text_color();
	set_text_color(gfx_3D_color);
	int c;
	for(c = 1; c <= gfx_3D_depth; c += gfx_3D_spacing)
		draw_text(int(x + c*(2.0*gfx_3D_angle_x/90.0)),
		          int(y + c*(2.0*gfx_3D_angle_y/90.0)), message);
	set_text_color(old_color);
	draw_text(x, y, message);
}
void draw_text_3D(Point p, std::string message)
{
	draw_text_3D(p.x, p.y, message);
}
void draw_text_3D(int x, int y, char ch)
{
	draw_text_3D(x, y, to_string(ch));
}
void draw_text_3D(Point p, char ch)
{
	draw_text_3D(p.x, p.y, to_string(ch));
}
void draw_text_3D(int x, int y, int n)
{
	draw_text_3D(x, y, to_string(n));
}
void draw_text_3D(Point p, int n)
{
	draw_text_3D(p.x, p.y, n);
}
void draw_text_3D(int x, int y, unsigned int n)
{
	draw_text_3D(x, y, to_string(n));
}
void draw_text_3D(Point p, unsigned int n)
{
	draw_text_3D(p.x, p.y, n);
}
void draw_text_3D(int x, int y, double n)
{
	draw_text_3D(x, y, to_string(n));
}
void draw_text_3D(Point p, double n)
{
	draw_text_3D(p.x, p.y, n);
}
void draw_text_P3D(int x, int y, std::string message)
{
	Color save_color = get_text_color();
	int save_size = get_text_size();
	set_text_color(gfx_3D_color);

	int depth;
	Point p;
	double dist_to_vp = distance(x, y, gfx_3D_vanishing_point);
	int max_depth = gfx_3D_depth;
	if( dist_to_vp < gfx_3D_depth)
		max_depth = int( dist_to_vp );

	for(depth = 1; depth <= max_depth; depth += gfx_3D_spacing)
	{
		p = circle_point(x, y, depth, gfx_3D_vanishing_point);
		//The ceil is the best that I can do to alleviate the problem 
		//that font size must come in integral units.
		set_text_size(int(ceil(distance(VanPt, p) / dist_to_vp * save_size)));
		draw_text(p, message);
	}
	set_text_color(save_color);
	set_text_size(save_size);
	draw_text(x, y, message);
}
void draw_text_P3D(Point p, std::string message)
{
	draw_text_P3D(p.x, p.y, message);
}
void draw_text_P3D(int x, int y, char ch)
{
	draw_text_P3D(x, y, to_string(ch));
}
void draw_text_P3D(Point p, char ch)
{
	draw_text_P3D(p.x, p.y, ch);
}
void draw_text_P3D(int x, int y, int n)
{
	draw_text_P3D(x, y, to_string(n));
}
void draw_text_P3D(Point p, int n)
{
	draw_text_P3D(p.x, p.y, n);
}
void draw_text_P3D(int x, int y, unsigned int n)
{
	draw_text_P3D(x, y, to_string(n));
}
void draw_text_P3D(Point p, unsigned int n)
{
	draw_text_P3D(p.x, p.y, n);
}
void draw_text_P3D(int x, int y, double n)
{
	draw_text_P3D(x, y, to_string(n));
}
void draw_text_P3D(Point p, double n)
{
	draw_text_P3D(p.x, p.y, n);
}
Point ellipse_point(Point left_top, Point right_bottom, int angle, int rot)
{
	return ellipse_point((left_top.x + right_bottom.x)/2, 
		                 (left_top.y + right_bottom.y)/2,
						 (right_bottom.x - left_top.x)/2,
						 (right_bottom.y - left_top.y)/2,
						 angle, rot);
}
Point ellipse_point(int x, int y, int rx, int ry, int angle, int rot)
{
	if(angle >= 360)
		angle = angle - 360 * (angle / 360);
	else if(angle < 0)
		angle = 360 + angle - 360 * (angle / 360);

	Point returnPt;

	if(angle == 90) returnPt = Point(x, y - ry);
	else if(angle == 270) returnPt = Point(x,y + ry);
	else if(angle == 180) returnPt = Point(x-rx,y);
	else if(angle == 360 || angle == 0 ) returnPt = Point(x+rx,y);
	else
	{
		double rad = angle * DEG2RAD;
		double dx, dy;
		if(90 <= angle && angle <= 270)
			dx = - 1.0 / sqrt(1.0/(rx*rx)+pow(tan(rad)/double(ry),2));
		else
			dx = 1.0 / sqrt(1.0/(rx*rx)+pow(tan(rad)/double(ry),2));
		if(angle <= 180)
			dy = double(ry) * sqrt(1.0-pow(double(dx)/double(rx),2));
		else
			dy = - double(ry) * sqrt(1.0-pow(double(dx)/double(rx),2));
		returnPt = Point(int(x + dx), int(y - dy));
	}
	if(rot == 0)
		return returnPt;
	else
		return rotate(returnPt, x, y, rot);
}
Point ellipse_point(Point center, int rx, int ry, int angle, int rot)
{
	return ellipse_point(center.x, center.y, rx, ry, angle, rot);
}
Point ellipse_point(Point left_top, Point right_bottom, Point ray_point, int rot)
{
	return ellipse_point((left_top.x + right_bottom.x)/2, 
		                 (left_top.y + right_bottom.y)/2,
						 (right_bottom.x - left_top.x)/2,
						 (right_bottom.y - left_top.y)/2,
						 ray_point, rot);
}
Point ellipse_point(int x, int y, int rx, int ry, Point ray_point, int rot)
{
	return ellipse_point(x, y, rx, ry, int(circle_angle(x, y, ray_point)), rot);
}
Point ellipse_point(Point center, int rx, int ry, Point ray_point, int rot)
{
	return ellipse_point(center.x, center.y, rx, ry, ray_point, rot);
}
void erase()
{
	FillRect(gfxe_hdc[gfx_active_page], &gfxe_screen_rectangle, gfxe_background_brush);
	if(gfx_active_page == 0)
		FillRect(gfxe_hdc[3], &gfxe_screen_rectangle, gfxe_background_brush);
}
void erase(Color color)
{
	set_background_color(color);
	erase();
}
void erase(int r, int g, int b)
{
	erase(Color(r,g,b));
}
void erase(int x, int y, int rx, int ry)
{
	erase(Point(x - rx, y - ry), Point(x + rx, y + ry));
}
void erase(Point center, int rx, int ry)
{
	erase(center.x, center.y, rx, ry);
}
void erase(Point left_top, Point right_bottom)
{
	//The FillRect function does not include its right and bottom
	RECT rect;
	rect.left = left_top.x;	rect.top = left_top.y;
	rect.right = right_bottom.x + 1;	rect.bottom = right_bottom.y + 1;

	FillRect(gfxe_hdc[gfx_active_page], &rect, gfxe_background_brush);
	if(gfx_active_page == 0)
		FillRect(gfxe_hdc[3], &rect, gfxe_background_brush);
}

void erase_buffer()
{
	int page = get_active_page();
	set_active_page(BUFFER);
	erase();
	set_active_page(page);
}
void erase_buffer(Color color)
{
	int page = get_active_page();
	set_active_page(BUFFER);
	erase(color);
	set_active_page(page);
}
void erase_buffer(int r, int g, int b)
{
	int page = get_active_page();
	set_active_page(BUFFER);
	erase(r, g, b);
	set_active_page(page);
}
void erase_buffer(int x, int y, int rx, int ry)
{
	int page = get_active_page();
	set_active_page(BUFFER);
	erase(x, y, rx, ry);
	set_active_page(page);
}
void erase_buffer(Point center, int rx, int ry)
{
	erase_buffer(center.x, center.y, rx, ry);
}
void erase_buffer(Point left_top, Point right_bottom)
{
	int page = get_active_page();
	set_active_page(BUFFER);
	erase(left_top, right_bottom);
	set_active_page(page);
}
void erase_buffer2()
{
	int page = get_active_page();
	set_active_page(BUFFER2);
	erase();
	set_active_page(page);
}
void erase_buffer2(Color color)
{
	int page = get_active_page();
	set_active_page(BUFFER2);
	erase(color);
	set_active_page(page);
}
void erase_buffer2(int r, int g, int b)
{
	int page = get_active_page();
	set_active_page(BUFFER2);
	erase(r, g, b);
	set_active_page(page);
}
void erase_buffer2(int x, int y, int rx, int ry)
{
	int page = get_active_page();
	set_active_page(BUFFER2);
	erase(x, y, rx, ry);
	set_active_page(page);
}
void erase_buffer2(Point center, int rx, int ry)
{
	erase_buffer2(center.x, center.y, rx, ry);
}
void erase_buffer2(Point left_top, Point right_bottom)
{
	int page = get_active_page();
	set_active_page(BUFFER2);
	erase(left_top, right_bottom);
	set_active_page(page);
}
void erase_screen()
{
	int page = get_active_page();
	set_active_page(SCREEN);
	erase();
	set_active_page(page);
}
void erase_screen(Color color)
{
	int page = get_active_page();
	set_active_page(SCREEN);
	erase(color);
	set_active_page(page);
}
void erase_screen(int r, int g, int b)
{
	int page = get_active_page();
	set_active_page(SCREEN);
	erase(r, g, b);
	set_active_page(page);
}
void erase_screen(int x, int y, int rx, int ry)
{
	int page = get_active_page();
	set_active_page(SCREEN);
	erase(x, y, rx, ry);
	set_active_page(page);
}
void erase_screen(Point center, int rx, int ry)
{
	erase_screen(center.x, center.y, rx, ry);
}
void erase_screen(Point left_top, Point right_bottom)
{
	int page = get_active_page();
	set_active_page(SCREEN);
	erase(left_top, right_bottom);
	set_active_page(page);
}
void flood_fill(int x, int y)
{
	flood_fill(x, y, gfx_pen_color);
}
void flood_fill(Point fillPt)
{
	flood_fill(fillPt.x, fillPt.y, gfx_pen_color);
}
void flood_fill(int x, int y, Color border_color)
{
	FloodFill(gfxe_hdc[gfx_active_page], x, y, 
		    RGB(border_color.r, border_color.g, border_color.b));
	if(gfx_active_page == 0)
		FloodFill(gfxe_hdc[3], x, y, 
			RGB(border_color.r, border_color.g, border_color.b));
}
void flood_fill(Point fillPt, Color border_color)
{
	flood_fill(fillPt.x, fillPt.y, border_color);
}
void flood_fill(int x, int y, int r, int g, int b)
{
	flood_fill(x, y, Color(r, g, b));
}
void flood_fill(Point fillPt, int r, int g, int b)
{
	flood_fill(fillPt, Color(r, g, b));
}
bool is_char()
{
	return get_char(false, false, false) != 0;
}
bool is_click()
{
	Point p = get_click(false, false, false);
	return !(p.x == -1 && p.y == -1);
}
int get_3D_angle_x()
{	
	return gfx_3D_angle_x;	
}
int get_3D_angle_y()
{	
	return gfx_3D_angle_y;
} 
Color get_3D_color(){
	return gfx_3D_color;
}
int	get_3D_depth(){
	return gfx_3D_depth;
}
int get_3D_flare()
{	
	return gfx_3D_flare;
}
int	get_3D_spacing()
{	
	return gfx_3D_spacing;
}
int get_3D_twist()
{
	return gfx_3D_twist;
}
Point get_3D_vanishing_point()
{
	return gfx_3D_vanishing_point;
}
int get_active_page()
{
	return gfx_active_page;
}
int get_animation_easing()
{
	return gfx_animation_easing;
}
bool get_animation_erase()
{
	return gfx_animation_erase;
}
int get_animation_frames_per_second()
{
	return gfx_animation_frames_per_second;
}
bool get_animation_repeat()
{
	return gfx_animation_repeat;
}
Color get_background_color()
{
	return gfx_background_color;
}
char get_char(bool wait, bool remove_prior, bool remove_returned)
{
	char return_char = 0;
	MSG msg;
	//Process everything in the message queue.  
	//gfx_last_char will then be the latest keypress
	while(PeekMessage(&msg, NULL, 0,0,PM_REMOVE))	//if?
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if(remove_prior)
		gfx_last_char = 0;
	if(wait && gfx_last_char == 0)
	{
		while(gfx_last_char == 0)
		{
			GetMessage(&msg, NULL, 0, 0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return_char = gfx_last_char;
	}
	else
		return_char = gfx_last_char;

	if(remove_returned)
		gfx_last_char = 0;

	return return_char;
}
Point get_click(bool wait, bool remove_prior, bool remove_returned)
{
	Point return_click_point = Point(-1,-1);
	MSG msg;
	//Process everything in the message queue.  
	//gfx_last_click_point will then be the latest
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if(remove_prior)
		gfx_last_click_point = Point(-1,-1);

	if(wait && gfx_last_click_point.x == -1 && gfx_last_click_point.y == -1)
	{
		while(gfx_last_click_point.x == -1 && gfx_last_click_point.y == -1)
		{
			GetMessage(&msg, NULL, 0, 0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return_click_point = gfx_last_click_point;
	}
	else
		return_click_point = gfx_last_click_point;

	if(remove_returned)
		gfx_last_click_point = Point(-1,-1);

	return return_click_point;
}
Color get_color(int x, int y)
{
	COLORREF c;
	//It is possible for Windows to update the screen during a call to get_color.
	//Then the call to get_color will return an inaccurate result.
	//So to be safe, we look on the copy buffer instead of the screen.
	//To be honest, I don't know if this is a real problem or not.
	if(gfx_active_page == 0)
		c = GetPixel(gfxe_hdc[3],x,y);
	else
		c = GetPixel(gfxe_hdc[gfx_active_page],x,y);
	return Color(GetRValue(c), GetGValue(c), GetBValue(c));
}
Color get_color(Point p)
{
	return get_color(p.x, p.y);
}

Color get_fill_color()
{
	return gfx_fill_color;
}

Color get_pen_color()
{
	return gfx_pen_color;
}
int get_pen_style()
{
	switch(gfx_pen_style)
	{
		case PS_SOLID: return SOLID_PEN;
		case PS_DOT: return DOT_PEN;
		case PS_DASH: return DASH_PEN;
		case PS_DASHDOT:  return DASH_DOT_PEN;		break;
		case PS_DASHDOTDOT: return DASH_DOT_DOT_PEN;	break;
		default: return SOLID_PEN;
	}
}
int get_pen_width()
{
	return gfx_pen_width;
}
Color get_pixel_color()
{
	return gfx_pixel_color;
}
int get_shape_sides()
{
	return Shape::max_sides;
}
int get_text_alignment()
{
	return gfx_text_alignment;
	//return GetTextAlign(gfxe_hdc[0]);
}
Color get_text_color()
{
	return gfx_text_color;
}
std::string get_text_font()
{
	return gfx_text_font;
	//Here is the way to access the font directly
	char szFont[32];
#ifdef _UNICODE
	GetTextFace
	(	gfxe_hdc[0],	// handle to device context
		32,			// length of buffer receiving typeface name
		(LPWSTR) szFont		// pointer to buffer receiving typeface name
	);
#else
		GetTextFace
	(	gfxe_hdc[0],		// handle to device context
		32,					// length of buffer receiving typeface name
		szFont				// pointer to buffer receiving typeface name
	);
#endif
	return std::string(szFont);
}
int get_text_rotation()
{
	return gfx_text_rotation;
}
int get_text_size()
{
	return gfx_text_size;
}
int message_box(std::string message, int buttons, int default_button, int icon)
{
	return message_box(message, "", buttons, default_button, icon);
}
int message_box(std::string message, std::string caption, int buttons, int default_button, int icon)
{
	unsigned int p[] = {unsigned int(buttons), unsigned int(default_button), unsigned int(icon)};
	unsigned int t = unsigned int(MB_OK);	//type of message box

	int i;
	for(i = 0; i < 3; i++)
	{
		switch(p[i])
		{
		case BUTTON_ABORT_RETRY_IGNORE: case BUTTONS_ABORT_RETRY_IGNORE:
			t = t | MB_ABORTRETRYIGNORE; break;
		case BUTTON_OK: case BUTTONS_OK:
			t = t | MB_OK; break;
		case BUTTON_OK_CANCEL: case BUTTONS_OK_CANCEL:
			t = t | MB_OKCANCEL; break;
		case BUTTON_RETRY_CANCEL: case BUTTONS_RETRY_CANCEL:
			t = t | MB_RETRYCANCEL; break;
		case BUTTON_YES_NO: case BUTTONS_YES_NO:
		    t = t | MB_YESNO; break;
		case BUTTON_YES_NO_CANCEL: case BUTTONS_YES_NO_CANCEL:
			t = t | MB_YESNOCANCEL;
		case ICON_EXCLAMATION:
			t = t | MB_ICONEXCLAMATION; break;
		case ICON_INFORMATION:
			t = t | MB_ICONINFORMATION; break;
		case ICON_QUESTION:
			t = t | MB_ICONQUESTION; break;
		case ICON_STOP:
			t = t | MB_ICONSTOP; break;
		case DEFAULT_BUTTON1:
			t = t | MB_DEFBUTTON1; break;
		case DEFAULT_BUTTON2:
			t = t | MB_DEFBUTTON2; break;
		case DEFAULT_BUTTON3:
			t = t | MB_DEFBUTTON3; break;
		case DEFAULT_BUTTON4:
			t = t | MB_DEFBUTTON4; break;
		}
	}
#ifdef _UNICODE
	//The following is needed because the project default is Unicode:
	//Project -> Properties -> Configuration -> General -> Character Set
	TCHAR* unicodePtr1 = new TCHAR[message.length() + 1];
	for(int i = 0; i < (int) message.length(); i++)
		unicodePtr1[i] = message[i];
	unicodePtr1[message.length()] = '\0';

	TCHAR* unicodePtr2 = new TCHAR[caption.length() + 1];
	for(int i = 0; i < (int) caption.length(); i++)
		unicodePtr2[i] = caption[i];
	unicodePtr2[caption.length()] = '\0';

	//TOPMOST keeps it from disappearing after the user clicks on the close box
	//(Here is where you might also affect the box's modality.)
	unsigned int ret = MessageBox(NULL, (LPCWSTR) unicodePtr1, (LPCWSTR) unicodePtr2, t | MB_TOPMOST);
	delete[] unicodePtr1;
	delete[] unicodePtr2;
#else
	unsigned int ret = MessageBox(NULL, message.c_str(), caption.c_str(), t | MB_TOPMOST);
#endif


	switch(ret)
	{
		case IDABORT: return BUTTON_ABORT;
		case IDCANCEL: return BUTTON_CANCEL;
		case IDIGNORE: return BUTTON_IGNORE;
		case IDNO: return BUTTON_NO;
		case IDOK: return BUTTON_OK;
		case IDRETRY: return BUTTON_RETRY;
		case IDYES: return BUTTON_YES;
		default: return 0;
	}

	return 0;
}


void open_window(std::string caption)
{
	open_window(-1, -1, caption);
}
void open_window(int width, int height, std::string caption)
{
	//A WINDCLASSEX is a structure that hold information about how Windows should be created.
	//First we create a WNDCLASSEX object, then we fill its fields, and finally we register
	//the structure.
	WNDCLASSEX wc;

	//We have to use a StockObject here because Colors are undefined
	gfxe_background_brush = (HBRUSH) GetStockObject(BLACK_BRUSH);

	//FILL IN THE WINDOW CLASS STRUCTURE (12 fields) AND REGISTER IT
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;// |CS_SAVEBITS;		//Window style
				//CS_OWNDC speeds up the process of painting to the window
				//Experiment with adding CS_SAVEBITS; consider CS_DBLCLKS.
	wc.hbrBackground = gfxe_background_brush;			//Background brush
	wc.hCursor  = LoadCursor(NULL, IDC_ARROW);		//Handle for mouse cursor
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);		//Handle to the window icon
	wc.hIconSm  = LoadIcon(NULL, IDI_APPLICATION);	//Handle to a smaller icon
	wc.hInstance = NULL;			//Handle to the application's instance
					//Set to NULL in console application, hInstance in WinApp

#ifdef _UNICODE
	//The following is needed because the project default is Unicode:
	//Project -> Properties -> Configuration -> General -> Character Set
		TCHAR* unicodePtr = new TCHAR[caption.length() + 1];
		for(int i = 0; i < (int) caption.length(); i++)
			unicodePtr[i] = caption[i];
		unicodePtr[caption.length()] = '\0';

		wc.lpszClassName = (LPCWSTR) unicodePtr;		//Text name for the WNDCLASSEX
		//delete[] unicodePtr;	This will be done later
#else
	wc.lpszClassName = caption.c_str();
#endif

	wc.lpszMenuName = NULL;			//Handle to the resource for our menu
	wc.lpfnWndProc = WndProc;			//The name of our event handler

	wc.cbSize = sizeof(WNDCLASSEX);		//The size of this structure in bytes
	wc.cbClsExtra = 0;	//Extra bytes to be stored with the structure
	wc.cbWndExtra = 0;	//Extra bytes to be stored with the windows

	RegisterClassEx  (&wc);

	//CREATE THE WINDOW BASED ON THE WNDCLASSEX STRUCTURE (12 parameters)
	gfxe_hwnd = CreateWindowEx
	(
	   WS_EX_LEFT,			//Advanced style settings
	   wc.lpszClassName,		//The name of the WNDCLASSEX structure above
#ifdef _UNICODE
	   (LPCWSTR) unicodePtr,
#else
	   caption.c_str(),
#endif
	   WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
	   //WS_OVERLAPPEDWINDOW,		//The window style
				//WS_OVERLAPPEDWINDOW combines WS_OVERLAPPED, WS_CAPTION,
				//WS_SYSMENU, WS_THICKFRAME, WS_MINIMIZEBOX, WS_MAXIMIZEBOX 
	   0, 0,			//The initial upper-left corner
       (width == -1 ? GetSystemMetrics(SM_CXSCREEN) : width),	//Initial width
	   (height == -1 ?GetSystemMetrics(SM_CYSCREEN) : height),	//Initial height
				//Note that you can use CW_USEDEFAULT for both the above
				//and the task bar will show	
	   (HWND) NULL,				//Handle to the parent window
	   (HMENU) NULL,			//Handle to the menu
	   NULL,				//Handle to the apps instance
	   NULL					//Advanced context
	);
#ifdef _UNICODE
	delete[] unicodePtr;	//Tidying up
#endif

	//Display the window we just created; sends WM_SIZE message
		//( //SW_SHOWMAXIMIZED shows the taskbar at the bottom)
    ShowWindow(gfxe_hwnd, SW_SHOWNORMAL);
	
	InitializeGrafiX();

	//Draw the window for the first time; sends WM_PAINT message
	UpdateWindow(gfxe_hwnd);

	//A last call to erase prevents the Windows taskbar from showing??
	erase();

	//stop_sound();	//Calling a sound function here prevents a compiler warning
}
bool point_in_rectangle(Point left_top, Point right_bottom, Point p)
{
	RECT rect;
	SetRect(&rect, left_top.x, left_top.y, right_bottom.x, right_bottom.y);
	POINT winPt;
	winPt.x = p.x;
	winPt.y = p.y;
	if(PtInRect(&rect, winPt) == 0)
		return false;
	return true;
}
bool point_in_rectangle(int x, int y, int rx, int ry, Point p)
{
	return point_in_rectangle(Point(x - rx, y - ry), Point(x + rx, y + ry),p);
}
bool point_in_rectangle(Point center, int rx, int ry, Point p)
{
	return point_in_rectangle(center.x, center.y, rx, ry, p);
}
/*The default is that play_sound will play a sound bundled as a resource with the executable.
But if the argument has the .wav extension, then play_sound will look for an 
external sound file of that name.*/
void play_sound(std::string sound, bool loop)
{
	int n = sound.length();
	bool is_filename  = ( n >= 4 && sound[n-4] == '.' &&
		tolower(sound[n-3]) == 'w' &&
		tolower(sound[n-2]) == 'a' &&
		tolower(sound[n-1]) == 'v');
	if(!is_filename)
	{
		play_bundled_sound(sound, loop);
		return;
	}
	
	//At this point we are playing an external file
	std::string sound_file = sound;

#ifdef _UNICODE
	//The following is needed because the project default is Unicode:
	//Project -> Properties -> Configuration -> General -> Character Set
	//(See also draw_text.)

		TCHAR* unicodePtr = new TCHAR[sound_file.length() + 1];
		for(int i = 0; i < (int) sound_file.length(); i++)
			unicodePtr[i] = sound_file[i];
		unicodePtr[sound_file.length()] = '\0';

	if(loop)
	   PlaySound((LPCWSTR) unicodePtr, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	else
	   PlaySound((LPCWSTR) unicodePtr, NULL, SND_FILENAME | SND_ASYNC);

	delete[] unicodePtr;
#else
	if(loop)
		PlaySound(sound_file.c_str(), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	else
		PlaySound(sound_file.c_str(), NULL, SND_FILENAME | SND_ASYNC);

#endif
	//PlaySound(sound_file.c_str(), NULL, SND_FILENAME | SND_ASYNC);
	//SND_ASYNC - The sound is played asynchronously and PlaySound returns 
	//immediately after beginning the sound. To terminate an asynchronously 
	//played waveform sound, call PlaySound with pszSound set to NULL. 
	//SND_FILENAME - The pszSound parameter is a filename. 
	//SND_LOOP - The sound plays repeatedly until PlaySound is called again
	//with the pszSound parameter (first) set to NULL. You must also specify
	//the SND_ASYNC flag to indicate an asynchronous sound event. 
	//SND_NOSTOP - The specified sound event will yield to another sound event
	//that is already playing. If a sound cannot be played because the resource
	//needed to generate that sound is busy playing another sound, the function
	//immediately returns FALSE without playing the requested sound. 
	//If this flag is not specified, PlaySound attempts to stop the currently 
	//playing sound so that the device can be used to play the new sound. 
	//SND_SYNC - Synchronous playback of a sound event. PlaySound returns after
	//the sound event completes. 
}
void play_bundled_sound(std::string sound_name, bool loop)
{
	//Originally the following line was #ifdef...
#ifndef _UNICODE
	//The following is needed because the project default is Unicode:
	//Project -> Properties -> Configuration -> General -> Character Set

		TCHAR* unicodePtr = new TCHAR[sound_name.length() + 1];
		for(int i = 0; i < (int) sound_name.length(); i++)
			unicodePtr[i] = sound_name[i];
		unicodePtr[sound_name.length()] = '\0';

	if(loop)
	   //PlaySound((LPCWSTR) unicodePtr, NULL, SND_RESOURCE | SND_ASYNC | SND_LOOP);//original
		PlaySound((LPCSTR) unicodePtr, NULL, SND_RESOURCE | SND_ASYNC | SND_LOOP);  //2009 change
	else
	   //PlaySound((LPCWSTR) unicodePtr, NULL, SND_RESOURCE | SND_ASYNC);//original
	   PlaySound((LPCSTR) unicodePtr, NULL, SND_RESOURCE | SND_ASYNC); //2009 change

	delete[] unicodePtr;
#else
	if(loop)
		PlaySound((LPCWSTR) sound_name.c_str(), NULL, SND_RESOURCE | SND_ASYNC | SND_LOOP);
	else
		PlaySound((LPCWSTR) sound_name.c_str(), NULL, SND_FILENAME | SND_ASYNC);

#endif
}
void quit(bool close_window_immediately)
{
	gfxe_close_flag = close_window_immediately;
	exit(0);
}
void quit_confirm(bool close_window_immediately)
{
	if(gfxe_program_done)
	{
		gfxe_close_flag = true;
		return;
	}

	//This only seems to bring up the message box when I quit in an ANIMATE...
	string m1 = "Do you want to quit?";
	string m2 = "Quit Confirmation";
#ifdef _UNICODE
	//The following is needed because the project default is Unicode:
	//Project -> Properties -> Configuration -> General -> Character Set

		TCHAR* unicodePtr1 = new TCHAR[m1.length() + 1];
		for(int i = 0; i < (int) m1.length(); i++)
			unicodePtr1[i] = m1[i];
		unicodePtr1[m1.length()] = '\0';

		TCHAR* unicodePtr2 = new TCHAR[m2.length() + 1];
		for(int i = 0; i < (int) m2.length(); i++)
			unicodePtr2[i] = m2[i];
		unicodePtr2[m2.length()] = '\0';


	if(MessageBox
	(	 NULL,		//Just set this to NULL
		 (LPCWSTR) unicodePtr1,			//Message
		 (LPCWSTR) unicodePtr2,			//Title bar caption
		 MB_TOPMOST | MB_YESNO		//TOPMOST keeps box on top when called
	) == IDYES)
	{
		gfxe_close_flag = close_window_immediately;
		exit(0);
	}
	delete[] unicodePtr1;
	delete[] unicodePtr2;
#else
	if(MessageBox
	(	 NULL,		//Just set this to NULL
		 m1.c_str(),			//Message
		 m2.c_str(),			//Title bar caption
		 MB_TOPMOST | MB_YESNO		//TOPMOST keeps box on top when called
	) == IDYES)
	{
		gfxe_close_flag = close_window_immediately;
		exit(0);
	}
#endif
}

int random(int high)			//from 1 to high
{
	if(high <= 0) return 0;
	return rand()%high + 1;
}
int random(int low, int high)
{
	if(low == high) return low;
	if(low > high) std::swap(low, high);
	return rand()%(high - low + 1) + low;
}
double random_continuous(double low, double high, double increment)
{
	if(low == high) return low;
	if(low > high) std::swap(low, high);
	while((high - low) < increment)
		increment = increment / 10;
	if(increment == 0)
		return low;
	int bins = int((high - low) / increment);
	int n = random(1, bins);
	return low + n * increment;
}
Color random_blue (int low, int high)
{
	return Color(0,0,random(low,high));
}
Color random_color()
{
	return Color(rand()%256, rand()%256, rand()%256);
}
Color random_color(Color low, Color high)
{
	return Color(random(low.r,high.r), random(low.g,high.g), random(low.b,high.b));
}
Color random_green(int low, int high)
{
	return Color(0,random(low,high),0);
}
Point random_point()
{
	return Point(rand()%screen_width(), rand()%screen_height());
}
Point random_point(int x, int y, int rx, int ry)
{
	return random_point(Point(x - rx, y - ry), Point(x + rx, y + ry));
}
Point random_point(Point center, int rx, int ry)
{
	return random_point(center.x, center.y, rx, ry);
}
Point random_point(Point left_top, Point right_bottom)
{
	return Point(random(left_top.x, right_bottom.x),
		         random(left_top.y, right_bottom.y));
}
Point random_point(int inset_x, int inset_y)
{
	return random_point(Point(inset_x,inset_y),Point(MaxPt - Point(inset_x,inset_y)));
}
Color random_red(int low, int high)
{
	return Color(random(low,high),0,0);
}
RoundedShape random_rounded_shape(
				   int x_min, int x_max,
				   int y_min, int y_max,
				   int rx_min, int rx_max,
				   int ry_min, int ry_max,
				   int ns_min, int ns_max,
				   int out_min, int out_max)
{
	int n;
	if(ns_min < 3 && ns_max < 3)
		n = 0;
	else
		n = random((ns_min <= ns_max ? ns_min : ns_max), (ns_max >= ns_min ? ns_max : ns_min));
	if(n < 3)
		n = 0;
	return RoundedShape(random(x_min, x_max),
		         random(y_min, y_max),
				 random(rx_min, rx_max),
				 random(ry_min, ry_max),
				 n,
				 random(out_min, out_max));
}
RoundedShape random_rounded_shape(RoundedShape lim1, RoundedShape lim2)
{
	return random_rounded_shape(lim1.x, lim2.x, lim1.y, lim2.y, lim1.rx, lim2.rx,
		lim1.ry, lim2.ry, lim1.num_sides, lim2.num_sides, lim1.out, lim2.out);
}
Shape random_shape(int x_min, int x_max,
				   int y_min, int y_max,
				   int rx_min, int rx_max,
				   int ry_min, int ry_max,
				   int ns_min, int ns_max,
				   int rot_min, int rot_max, 
				   int out_min, int out_max)
{
	int n;
	if(ns_min < 3 && ns_max < 3)
		n = 0;
	else if(ns_min >= 3 && ns_max >= 3)
		n = random(ns_min, ns_max);
	else
	{
		n = random(2, ns_max >= ns_min ? ns_max : ns_min);
		if( n < 3)
			n = 0;
	}
	return Shape(random(x_min, x_max),
		         random(y_min, y_max),
				 random(rx_min, rx_max),
				 random(ry_min, ry_max),
				 n,
				 random(rot_min, rot_max),
				 random(out_min, out_max));
}
Shape random_shape(Shape lim1, Shape lim2)
{
	return random_shape(lim1.x, lim2.x, lim1.y, lim2.y, lim1.rx, lim2.rx,
		lim1.ry, lim2.ry, lim1.num_sides, lim2.num_sides, lim1.rot, lim2.rot, 
		lim1.out, lim2.out);
}
int random_x()
{
	return rand()%screen_width();
}
int random_y()
{
	return rand()%screen_height();
}
//Ranges over intervals
Color range(Color start, Color end, double proportion)
{
	return Color
		(
			range(start.r, end.r, proportion),
			range(start.g, end.g, proportion),
			range(start.b, end.b, proportion)
		);

}
int range(int start, int end, double proportion)
{
	return int(start + (end - start) * proportion);
}
Point range(Point start, Point end, double proportion)
{
	return Point(range(start.x, end.x, proportion), range(start.y, end.y, proportion));
}
double range_continuous(double start, double end, double proportion)
{
	return start + (end - start) * proportion;
}

RoundedShape range(RoundedShape start, RoundedShape stop, double proportion)
{
	int new_n = 0, new_out = 0;
	if( (start.num_sides < 3) && (stop.num_sides < 3) )
		new_n = new_out = 0;

	else if( (start.num_sides == stop.num_sides) && (start.out * stop.out > 0) )
	{
		new_n = start.num_sides;
		new_out = range(start.out, stop.out);
	}

	else
	{
		int step = range(-abs(start.out), abs(stop.out));
		if(step < 0){ new_n = start.num_sides; new_out = (start.out < 0 ? step : -step);}
		else if(step > 0){ new_n = stop.num_sides; new_out = (stop.out > 0 ? step : -step);}
		else {new_n = start.num_sides; new_out = 0;}
	}
	return RoundedShape(range(start.x, stop.x, proportion), range(start.y, stop.y, proportion),
		                range(start.rx, stop.rx, proportion), range(start.ry, stop.ry, proportion),
						new_n, new_out);
}
Shape range(Shape start, Shape end, double proportion)
{
	int new_n1 = (start.num_sides < 3 ? Shape::max_sides : start.num_sides);
	int new_n2 = (end.num_sides < 3 ? Shape::max_sides : end.num_sides);
	return Shape(range(start.x, end.x, proportion), range(start.y, end.y, proportion), range(start.rx, end.rx, proportion),
		         range(start.ry, end.ry, proportion), range(new_n1, new_n2, proportion),
				 range(start.rot, end.rot, proportion), range(start.out, end.out, proportion));
}
void remove_char()
{
	get_char(false, true, true);
}
void remove_click()
{
	get_click(false, true, true);
}

Point rotate(Point p, int x, int y, int angle)
{
	Point r;
	double c = cos(angle * DEG2RAD), s = - sin(angle * DEG2RAD);
	r.x = int(x + c * (p.x - x) - s * (p.y - y));
	r.y = int(y + s * (p.x - x) + c * (p.y - y));
	return r;
}
Point rotate(Point p, Point center, int angle)
{
	return rotate(p, center.x, center.y, angle);
}
void rotate(Point* point_array, int num_points, int x, int y, int angle)
{
	int i;
	for(i=0 ; i<num_points ; i++)
		point_array[i] = rotate(point_array[i], x, y, angle);
}
void rotate(Point* point_array, int num_points, Point center, int angle)
{
	rotate(point_array, num_points, center.x, center.y, angle);
}
int round(double d)
{
	return d - floor(d) >= .5 ? int(ceil(d)) : int(floor(d));
}
int run_time()
{
	return GetTickCount() - gfxe_begin_time;
}
void set_3D_angle_x(int angle)
{
	gfx_3D_angle_x = angle;
}
void set_3D_angle_y(int angle)
{
	gfx_3D_angle_y = angle;
} 
void set_3D_color(Color color)
{
	gfx_3D_color = color;
}
void set_3D_color(int r, int g, int b)
{
	set_3D_color(Color(r, g, b));
}
void set_3D_depth(int depth)
{
	gfx_3D_depth = depth;
}
void set_3D_flare(int flare)
{
	gfx_3D_flare = flare;
}
void set_3D_spacing(int spacing)
{
	if(spacing < 1) spacing = 1;
	gfx_3D_spacing = spacing;
}
void set_3D_twist(int twist)
{
	gfx_3D_twist = twist;
}
void set_3D_vanishing_point(Point vanishingPt)
{
	gfx_3D_vanishing_point = vanishingPt;
}
void set_active_page(int num)
{
	gfx_active_page = num;
}
void set_animation_easing(int easing)
{
	gfx_animation_easing = easing;
}
void set_animation_erase(bool status)
{
	gfx_animation_erase = status;
}
void set_animation_frames_per_second(int frames_per_second)
{
	//This function is a friend of the Animator class.

	//Don't reset frames per second below 1 or to current value
	if((frames_per_second < 1) || (frames_per_second == get_animation_frames_per_second()))
		return;

	//Save and then reset
	int old_frames_per_second = get_animation_frames_per_second();
	int new_frames_per_second = gfx_animation_frames_per_second = frames_per_second;

	//Calculate what the new frame# would have been at this new frames_per_second.
	int old_frame_on_buffer = Animator::current -> frame_on_buffer;
	int sec_run = old_frame_on_buffer / old_frames_per_second;
	int new_frame_on_buffer = Animator::current -> frame_on_buffer 
							= new_frames_per_second * sec_run;

	//Calculate what the new total frames would have to be in order to 
	//have the same remaining time at this new frames_per_second.
	int old_total_frames = Animator::current -> total_frames;
	int total_time = old_total_frames / old_frames_per_second;
	int sec_remaining = total_time - sec_run;	//could be negative

	Animator::current -> total_frames =
		new_frame_on_buffer + new_frames_per_second * sec_remaining;

}
void set_animation_repeat(bool status)
{
	gfx_animation_repeat = status;
}
void set_background_color(Color color)
{
	gfx_background_color = color;
	DeleteObject(gfxe_background_brush);
	gfxe_background_brush = CreateSolidBrush(RGB(color.r, color.g, color.b));
}
void set_background_color(int r, int g, int b)
{
	set_background_color(Color(r, g, b));
}
void set_color(Color color)
{
	set_fill_color(color);
	set_pen_color(color);
	set_text_color(color);
}
void set_color(int r, int g, int b)
{
	set_color(Color(r, g, b));
}

void set_fill_color(Color color)
{
	gfx_fill_color = color;

	HBRUSH hbrush_old = (HBRUSH) GetCurrentObject( gfxe_hdc[0], OBJ_BRUSH );
	LOGBRUSH logbrush;
	GetObject(hbrush_old, sizeof(LOGBRUSH), (LPVOID) & logbrush);
	logbrush.lbColor = RGB(color.r,color.g,color.b);
	HBRUSH hbrush_new = CreateBrushIndirect(&logbrush);
	int c;
	for(c = 0; c < 4; c++)
		SelectObject(gfxe_hdc[c], hbrush_new);
	DeleteObject(hbrush_old);

}
void set_fill_color(int r, int g, int b)
{
	set_fill_color(Color(r,g,b));
}
void set_pen_color(Color color)
{
	set_pixel_color(color);

	gfx_pen_color = color;

	HPEN hpen_old = (HPEN) GetCurrentObject( gfxe_hdc[0], OBJ_PEN );
	LOGPEN logpen;
	GetObject(hpen_old, sizeof(LOGPEN), (LPVOID) & logpen);
	logpen.lopnColor = RGB(color.r,color.g,color.b);
	HPEN hpen_new = CreatePenIndirect(&logpen);
	int c;
	for(c = 0; c < 4; c++)
		SelectObject(gfxe_hdc[c], hpen_new);
	DeleteObject(hpen_old);
}
void set_pen_color(int r, int g, int b)
{
	set_pen_color(Color(r,g,b));
}

void set_pen_style(int pen_style_enum)
{
	UINT PENSTYLE;
	switch(pen_style_enum)
	{
		case SOLID_PEN:			PENSTYLE = PS_SOLID;		break;
		case DOT_PEN:			PENSTYLE = PS_DOT;			break;
		case DASH_PEN:			PENSTYLE = PS_DASH;			break;
		case DASH_DOT_PEN:		PENSTYLE = PS_DASHDOT;		break;
		case DASH_DOT_DOT_PEN:	PENSTYLE = PS_DASHDOTDOT;	break;
		default:				PENSTYLE = PS_SOLID;		break;
	}

	HPEN hpen_old = (HPEN) GetCurrentObject( gfxe_hdc[0], OBJ_PEN );
	LOGPEN logpen;
	GetObject(hpen_old, sizeof(LOGPEN), (LPVOID) & logpen);
	logpen.lopnStyle = PENSTYLE;
	HPEN hpen_new = CreatePenIndirect(&logpen);
	int c;
	for(c = 0; c < 4; c++)
		SelectObject(gfxe_hdc[c], hpen_new);
	DeleteObject(hpen_old);

	if(PENSTYLE == PS_SOLID)
		gfx_pen_style = SOLID_PEN;
	else
		gfx_pen_style = pen_style_enum;
}
void set_pen_width(int pen_point_size)
{
	if(pen_point_size <= 0) return;
	else gfx_pen_width = pen_point_size;

	HPEN hpen_old = (HPEN) GetCurrentObject( gfxe_hdc[0], OBJ_PEN );
	LOGPEN logpen;
	GetObject(hpen_old, sizeof(LOGPEN), (LPVOID) & logpen);
	POINT p;
	p.x = pen_point_size;
	logpen.lopnWidth = p;
	HPEN hpen_new = CreatePenIndirect(&logpen);
	int c;
	for(c = 0; c < 4; c++)
		SelectObject(gfxe_hdc[c], hpen_new);
	DeleteObject(hpen_old);
}
void set_pixel_color(Color color)
{
	gfx_pixel_color = color;
}
void set_pixel_color(int r, int g, int b)
{
	gfx_pixel_color = Color(r,g,b);
}
void set_shape_sides(int num_sides)
{
	Shape::max_sides = (num_sides <= 0 ? 0 : num_sides);
}
void set_text_alignment(int align_enum)
{
	UINT TEXTALIGN;
	switch(gfx_text_alignment = align_enum)
	{
		case LEFT_TOP:		TEXTALIGN = TA_LEFT   | TA_TOP; break;
		case CENTER_TOP:	TEXTALIGN = TA_CENTER | TA_TOP; break;
		case RIGHT_TOP:		TEXTALIGN = TA_RIGHT  | TA_TOP; break;
		case LEFT_BASELINE:  TEXTALIGN = 	TA_LEFT   | TA_BASELINE; break;
		case CENTER_BASELINE: TEXTALIGN = TA_CENTER | TA_BASELINE; break;
		case RIGHT_BASELINE:  TEXTALIGN = TA_RIGHT  | TA_BASELINE; break;
		case LEFT_BOTTOM:	TEXTALIGN = TA_LEFT   | TA_BOTTOM; break;
		case CENTER_BOTTOM:	TEXTALIGN = TA_CENTER | TA_BOTTOM; break;
		case RIGHT_BOTTOM:	TEXTALIGN = TA_RIGHT  | TA_BOTTOM; break;
		default:			TEXTALIGN = TA_CENTER | TA_BASELINE;
							gfx_text_alignment = CENTER;	break;
	}
	int c;
	for(c = 0; c < 4; c++)
		SetTextAlign(gfxe_hdc[c], TEXTALIGN);
}
void set_text_color(Color color)
{
	gfx_text_color = color;

	int c;
	for(c = 0; c < 4; c++)
		SetTextColor(gfxe_hdc[c], RGB(color.r, color.g, color.b));
}
void set_text_color(int r, int g, int b)
{
	set_text_color(Color(r,g,b));
}
//If we pass an invalid string or no font_name, we get the system font.
void set_text_font(std::string font_name)
{
	gfx_text_font = font_name;
	//Don't delete a font while it is selected into a DC
	HFONT hfont_old = (HFONT) GetCurrentObject( gfxe_hdc[0], OBJ_FONT);
	LOGFONT logfont;
	GetObject(hfont_old, sizeof(LOGFONT), (LPVOID) & logfont);

#ifdef _UNICODE
	//The following is needed because the project default is Unicode:
	//Project -> Properties -> Configuration -> General -> Character Set
		TCHAR* unicodePtr = new TCHAR[font_name.length() + 1];
		for(int i = 0; i < (int) font_name.length(); i++)
			unicodePtr[i] = font_name[i];
		unicodePtr[font_name.length()] = '\0';

	lstrcpy(logfont.lfFaceName, (LPCWSTR) unicodePtr);
	delete[] unicodePtr;
#else
	lstrcpy(logfont.lfFaceName, font_name.c_str());
#endif
	logfont.lfCharSet = DEFAULT_CHARSET;	//Needed because SYMBOL changes it

	HFONT hfont_new = CreateFontIndirect(&logfont);

	int c;
	for(c = 0; c < 4; c++)
		SelectObject(gfxe_hdc[c], hfont_new);	//All 4 get same!!!
	DeleteObject(hfont_old);

}
void set_text_rotation(int angle)
{
	gfx_text_rotation = angle;

	HFONT hfont_old = (HFONT) GetCurrentObject( gfxe_hdc[0], OBJ_FONT);
	LOGFONT logfont;
	GetObject(hfont_old, sizeof (LOGFONT), (LPVOID) & logfont);
	logfont.lfEscapement = logfont.lfOrientation/*??*/ = angle * 10;

	HFONT hfont_new = CreateFontIndirect(&logfont);

	int c;
	for(c = 0; c < 4; c++)
		SelectObject(gfxe_hdc[c], hfont_new);
	DeleteObject(hfont_old);
}
void set_text_size(int points)
{
	if(points > 0)
		gfx_text_size = points;
	else
		return;

	//Don't delete a font while it is selected into a DC
	HFONT hfont_old = (HFONT) GetCurrentObject( gfxe_hdc[0], OBJ_FONT);
	LOGFONT logfont;
	GetObject(hfont_old, sizeof(LOGFONT), (LPVOID) & logfont);

	FLOAT cxDpi = (FLOAT) GetDeviceCaps(gfxe_hdc[0], LOGPIXELSX);
	FLOAT cyDpi = (FLOAT) GetDeviceCaps(gfxe_hdc[0], LOGPIXELSY);
	POINT pt;
	pt.x = 0;
	pt.y = (int) (points * cyDpi / 7.2);
	DPtoLP(gfxe_hdc[0],&pt,1);
	logfont.lfHeight = - (int) (abs (pt.y) / 10.0 + .05);

	HFONT hfont_new = CreateFontIndirect(&logfont);

	int c;
	for(c = 0; c < 4; c++)
		SelectObject(gfxe_hdc[c], hfont_new);
	DeleteObject(hfont_old);
}
int screen_height()
{
	return gfxe_screen_rectangle.bottom;
}
int screen_width()
{
	return gfxe_screen_rectangle.right;
}
void settings_reset()
{
	settings_3D_reset();
	settings_animation_reset();
	settings_fill_reset();
	settings_pen_reset();
	settings_text_reset();

	set_active_page(gfx_active_page_default);
	set_background_color(gfx_background_color_default);
	set_pixel_color(gfx_pixel_color_default);
}
void settings_3D_reset()
{
	set_3D_angle_x(gfx_3D_angle_x_default);
	set_3D_angle_y(gfx_3D_angle_y_default);
	set_3D_color(gfx_3D_color_default);
	set_3D_depth(gfx_3D_depth_default);
	set_3D_flare(gfx_3D_flare_default);
	set_3D_spacing(gfx_3D_spacing_default);
	set_3D_twist(gfx_3D_twist_default);
	set_3D_vanishing_point(gfx_3D_vanishing_point_default);
}
void settings_animation_reset()
{
	set_animation_easing(gfx_animation_easing_default);
	set_animation_erase(gfx_animation_erase_default);
	set_animation_frames_per_second(gfx_animation_frames_per_second_default);
	set_animation_repeat(gfx_animation_repeat_default);
}
void settings_fill_reset()
{
	set_fill_color(gfx_fill_color_default);
}
void settings_pen_reset()
{
	set_pen_color(gfx_pen_color_default);
	set_pen_style(gfx_pen_style_default);
	set_pen_width(gfx_pen_width_default);
}
void settings_text_reset()
{
	set_text_alignment(gfx_text_alignment_default);
	set_text_color(gfx_text_color_default);
	set_text_font(gfx_text_font_default);
	set_text_rotation(gfx_text_rotation_default);
	set_text_size(gfx_text_size_default);
}

void settings_restore()
{
	settings_3D_restore();
	settings_animation_restore();
	settings_fill_restore();
	settings_pen_restore();
	settings_text_restore();

	set_active_page(gfx_active_page_save);
	set_background_color(gfx_background_color_save);
	set_pixel_color(gfx_pixel_color_save);
}
void settings_3D_restore()
{
	set_3D_angle_x(gfx_3D_angle_x_save);
	set_3D_angle_y(gfx_3D_angle_y_save);
	set_3D_color(gfx_3D_color_save);
	set_3D_depth(gfx_3D_depth_save);
	set_3D_flare(gfx_3D_flare_save);
	set_3D_spacing(gfx_3D_spacing_save);
	set_3D_twist(gfx_3D_twist_save);
	set_3D_vanishing_point(gfx_3D_vanishing_point_save);
}
void settings_animation_restore()
{
	set_animation_easing(gfx_animation_easing_save);
	set_animation_erase(gfx_animation_erase_save);
	set_animation_frames_per_second(gfx_animation_frames_per_second_save);
	set_animation_repeat(gfx_animation_repeat_save);
}
void settings_fill_restore()
{
	set_fill_color(gfx_fill_color_save);
}
void settings_pen_restore()
{
	set_pen_color(gfx_pen_color_save);
	set_pen_style(gfx_pen_style_save);
	set_pen_width(gfx_pen_width_save);
}
void settings_text_restore()
{
	set_text_alignment(gfx_text_alignment_save);
	set_text_color(gfx_text_color_save);
	set_text_font(gfx_text_font_save);
	
	set_text_size(gfx_text_size_save);
	set_text_rotation(gfx_text_rotation_save);
}

void settings_save()
{
	settings_3D_save();
	settings_animation_save();
	settings_fill_save();
	settings_pen_save();
	settings_text_save();

	gfx_active_page_save = get_active_page();
	gfx_background_color_save = get_background_color();
	gfx_pixel_color_save = get_pixel_color();
}
void settings_3D_save()
{
	gfx_3D_angle_x_save = get_3D_angle_x();
	gfx_3D_angle_y_save = get_3D_angle_y();
	gfx_3D_color_save = get_3D_color();
	gfx_3D_depth_save = get_3D_depth();
	gfx_3D_flare_save = get_3D_flare();
	gfx_3D_spacing_save = get_3D_spacing();
	gfx_3D_twist_save = get_3D_twist();
	gfx_3D_vanishing_point_save = get_3D_vanishing_point();
}
void settings_animation_save()
{
	gfx_animation_easing_save = get_animation_easing();
	gfx_animation_erase_save = get_animation_erase();
	gfx_animation_frames_per_second_save = get_animation_frames_per_second();
	gfx_animation_repeat_save = get_animation_repeat();
}
void settings_fill_save()
{
	gfx_fill_color_save = get_fill_color();
}
void settings_pen_save()
{
	gfx_pen_color_save = get_pen_color();
	gfx_pen_style_save = get_pen_style();
	gfx_pen_width_save = get_pen_width();
}
void settings_text_save()
{
	gfx_text_alignment_save = get_text_alignment();
	gfx_text_color_save = get_text_color();
	gfx_text_font_save = get_text_font();
	gfx_text_rotation_save = get_text_rotation();
	gfx_text_size_save = get_text_size();
}
void stop_sound()
{
	PlaySound(NULL, NULL, SND_ASYNC);
}
int text_height(std::string s)
{
    SIZE ss;
#ifdef _UNICODE
    GetTextExtentPoint32(gfxe_hdc[0], (LPCWSTR) s.c_str(), (int)s.length(), &ss);
#else
    GetTextExtentPoint32(gfxe_hdc[0], s.c_str(), (int)s.length(), &ss);
#endif
    return ss.cy;
}
int text_width(std::string s)
{
    SIZE ss;
#ifdef _UNICODE
    GetTextExtentPoint32(gfxe_hdc[0], (LPCWSTR) s.c_str(), (int)s.length(), &ss);
#else
    GetTextExtentPoint32(gfxe_hdc[0], s.c_str(), (int)s.length(), &ss);
#endif
    return ss.cx;
}
double to_double(std::string str)
{
	return atof(str.c_str());
}
int to_int(std::string str)
{
	return atoi(str.c_str());
}
int to_int(char ch)
{
	if('0' <= ch && ch <= '9')
		return ch - '0';
	else
		return 0;
}
int to_int(double x)
{
	return (int) x;
}
std::string to_string(const char* str)
{
	return std::string(str);
}
std::string to_string(char ch)
{
	return string(1,ch);
}
std::string to_string(int n)
{
	char a[16];
	//warning said to change sprintf to sprintf_s
	sprintf_s(a,"%d",n);
	return std::string(a);
}

std::string to_string(unsigned int n)
{
	return to_string(int(n));
}

std::string to_string(double n)
{
	char a[16];
	//warning said to change sprintf to sprintf_s
	sprintf_s(a,"%f",n);

	//Terminate any trailing zeros after the decimal              
	int null_pos = 0;
	while(a[null_pos] != '\0') null_pos++;
	int dec_pos;
	for(dec_pos = null_pos - 1; dec_pos >= 0; dec_pos--)
		if(a[dec_pos] == '.') break;
	if(dec_pos != -1)
	{
		int zero_pos = null_pos - 1;
		while(a[zero_pos] == '0')
		{
			a[zero_pos] = '\0';
			zero_pos--;
		}
	}

	return std::string(a);
}
void vertices_elliptic_ngon(Point* vertex_array, int x, int y, int rx, int ry, int ns, int rot)
{
	vertices_ngon(vertex_array, x, y, rx, ns);
	//stretch these vertices to become corresponding ellipse vertices
	if(rx > 0)
		for(int c = 0; c < ns; c++)
			vertex_array[c].y = y +(vertex_array[c].y - y) * ry / rx;
	//rotate these points if necessary:
	if(rot != 0)
		rotate(vertex_array, ns, x, y, rot);
}
void vertices_elliptic_ngon(Point* vertex_array, Point center, int rx, int ry, int ns, int rot)
{
	vertices_elliptic_ngon(vertex_array, center.x, center.y, rx, ry, ns, rot);
}
void vertices_elliptic_outgon(Point* vertex_array, int x, int y, int rx, int ry, int ns, int rot)
{
	vertices_outgon(vertex_array, x, y, rx, ns);
	//stretch these vertices to become corresponding ellipse vertices
	if(rx > 0)
		for(int c = 0; c < ns; c++)
			vertex_array[c].y = y + (vertex_array[c].y - y) * ry / rx;
	//rotate these points if necessary:
	if(rot!=0)
		rotate(vertex_array, ns, x, y, rot);
}
void vertices_elliptic_outgon(Point* vertex_array, Point center, int rx, int ry, int ns, int rot)
{
	vertices_elliptic_outgon(vertex_array, center.x, center.y, rx, ry, ns, rot);
}
void vertices_ngon(Point* vertex_array, int x, int y, int r, int ns, int rot)
{
	//This is the angle between vertices in degrees
	double angle = 360.0 / ns;
	
	//fa is the first angle chosen so the base is horizontal if rot = 0
	double fa;
	 if(angle < 1)
		fa = rot;
	else
		fa = int(270 - 180.0 / ns) % int(angle) + rot;

	for(int c = 0; c < ns; c++)
		vertex_array[c] = circle_point(x, y, r, int(fa + c * angle));
}
void vertices_ngon(Point* vertex_array, Point center, int r, int ns, int rot)
{
	vertices_ngon(vertex_array, center.x, center.y, r, ns, rot);
}
void vertices_outgon(Point* vertex_array, int x, int y, int r, int ns, int rot)
{
	vertices_ngon(vertex_array, x, y, int(r / cos(PI / ns)), ns, rot);
}
void vertices_outgon(Point* vertex_array, Point center, int r, int ns, int rot)
{
	vertices_outgon(vertex_array, center.x, center.y, r, ns, rot);
}
/////////////////////////////////////////////////////////////////
int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	srand(int(time(NULL)));			//Initialize random number generator
	gfxe_begin_time = GetTickCount();
	open_window();
									
	//Trick computer into compiling with gdi32, user32, and winmm libraries
	//to avoid compiler warnings if user don't use these libraries.
	int y = rand() % 10;
	if(y == 20)	//Never true, but the compiler can't figure this out!
		stop_sound();

	int main();	//Prototype
	main();

	if(!gfxe_close_flag)
	{
		gfxe_program_done = true;
		remove_char();
		remove_click();
		while(!is_click() && !is_char() && !gfxe_close_flag);
	}
	close_window();

	return 0;
}
////////////////////////////////////////////////////////////////////////
void GrafiX_Welcome()
{
	//ANIMATE( 4, false )
	//{
	//	set_text_size(range(8, 48));
	//	set_text_color(BLUE);
	//	draw_text( range(PerPt(50,25), PerPt(50,75)),
	//		       "Welcome to GrafiX");
	//}
	set_text_size(48);
	//ANIMATE( 2, false )
	//{
	//	set_text_color(BLUE);
	//	set_3D_depth(range(1, MidY));
	//	draw_text_P3D( PerPt(50,75), "Welcome to GrafiX");
	//}
	
	ANIMATE( 4, false )
	{
		//set_3D_depth(30);
		draw_text_P3D( PerPt(50, 75), "Welcome to GrafiX");
		set_text_size(12);
		draw_text(PerPt(50, 80), "Press Escape to Exit");
		set_text_size(48);
		set_3D_twist(range_cycle(-45, 45));
		set_3D_depth(40);
		draw_square_3D_filled(PerPt(75, 30), 50);
		//set_3D_depth(MidY);
	}
}



