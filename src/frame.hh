// -*- mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; -*-
#ifndef   __frame_hh
#define   __frame_hh

/*! @file frame.hh
*/

extern "C" {
#include <X11/Xlib.h>
}

#include "python.hh"
#include "otk/strut.hh"
#include "otk/rect.hh"
#include "otk/renderstyle.hh"
#include "otk/ustring.hh"
#include "otk/surface.hh"
#include "otk/eventhandler.hh"

#include <string>

namespace ob {

class Client;

//! Varius geometry settings in the frame decorations
struct FrameGeometry {
  unsigned int width; // title and handle
  unsigned int font_height;
  unsigned int title_height() { return font_height + bevel*2; }
  unsigned int label_width;
  unsigned int label_height() { return font_height; }
  unsigned int handle_height; // static, from the style
  int handle_y;
  unsigned int button_size;   // static, from the style
  unsigned  grip_width() { return button_size * 2; }
  unsigned  bevel;         // static, from the style
  unsigned  bwidth;  // frame elements' border width
  unsigned  cbwidth; // client border width
};

//! Holds and decorates a frame around an Client (client window)
/*!
  The frame is responsible for calling XSelectInput on the client window's new
  parent with the SubstructureRedirectMask so that structure events for the
  client are sent to the window manager.
*/
class Frame : public otk::StyleNotify, public otk::EventHandler {
public:

  //! The event mask to grab on frame windows
  static const long event_mask = EnterWindowMask | LeaveWindowMask;
   
private:
  Client *_client;

  //! The size of the frame on each side of the client window
  otk::Strut _size;

  //! The size of the frame on each side of the client window inside the border
  otk::Strut _innersize;

  //! The position and size of the entire frame (including borders)
  otk::Rect _area;

  bool _visible;
  
  // decoration windows
  Window  _frame;   // sits under everything
  Window  _plate;   // sits entirely under the client window
  Window  _title;   // the titlebar
  Window  _label;   // the section of the titlebar which shows the window name
  Window  _handle;  // bottom bar
  Window  _lgrip;   // lefthand resize grab on the handle
  Window  _rgrip;   // righthand resize grab on the handle
  Window *_buttons; // all of the titlebar buttons
  unsigned int  _numbuttons; // number of buttons, size of _buttons array
  unsigned int *_titleorder; // order of the buttons and the label (always
                             // holds '_numbuttons + 1' elements (for the
                             // label, which is coded as '-1')

  // surfaces for each 
  otk::Surface  *_frame_sur;
  otk::Surface  *_title_sur;
  otk::Surface  *_label_sur;
  otk::Surface  *_handle_sur;
  otk::Surface  *_grip_sur;
  otk::Surface **_buttons_sur;

  FrameGeometry geom;
  
  void applyStyle(const otk::RenderStyle &style);
  void layoutTitle();
  void renderLabel();

public:
  //! Constructs an Frame object for a client
  /*!
    @param client The client which will be decorated by the new Frame
  */
  Frame(Client *client);
  //! Destroys the Frame object
  virtual ~Frame();

  //! Returns the size of the frame on each side of the client
  const otk::Strut& size() const { return _size; }
  
  //! Set the style to decorate the frame with
  virtual void styleChanged(const otk::RenderStyle &style);

  //! Reparents the client window from the root window onto the frame
  void grabClient();
  //! Reparents the client window back to the root window
  void releaseClient();

  //! Update the frame's size to match the client
  void adjustSize();
  //! Update the frame's position to match the client
  void adjustPosition();
  //! Shape the frame window to the client window
  void adjustShape();
  //! Update the frame to match the client's new state (for things like toggle
  //! buttons, focus, and the title) XXX break this up
  void adjustState();
  void adjustFocus();
  void adjustTitle();

  //! Applies gravity to the client's position to find where the frame should
  //! be positioned.
  /*!
    @return The proper coordinates for the frame, based on the client.
  */
  void clientGravity(int &x, int &y);

  //! Reversly applies gravity to the frame's position to find where the client
  //! should be positioned.
  /*!
    @return The proper coordinates for the client, based on the frame.
  */
  void frameGravity(int &x, int &y);

  //! The position and size of the frame window
  inline const otk::Rect& area() const { return _area; }

  //! Returns if the frame is visible
  inline bool visible() const { return _visible; }

  //! Shows the frame
  void show();
  //! Hides the frame
  void hide();

  //! Returns the MouseContext for the given window id
  /*!
    Returns '-1' if no valid mouse context exists in the frame for the given
    id.
  */
  ob::MouseContext::MC mouseContext(Window win) const;

  //! Gets the window id of the frame's base top-level parent
  inline Window window() const { return _frame; }
  //! Gets the window id of the client's parent window
  inline Window plate() const { return _plate; }

  //! Returns a null terminated array of the window ids that make up the
  //! frame's decorations.
  Window *allWindows() const;
};

}

#endif // __frame_hh
