// Configmenu.cc for Openbox
// Copyright (c) 2002 - 2002 Ben Jansens <ben@orodu.net>
// Copyright (c) 2001 Sean 'Shaleh' Perry <shaleh@debian.org>
// Copyright (c) 1997 - 2000 Brad Hughes (bhughes@tcac.net)
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

// stupid macros needed to access some functions in version 2 of the GNU C
// library
#ifndef   _GNU_SOURCE
#  define _GNU_SOURCE
#endif // _GNU_SOURCE

#ifdef    HAVE_CONFIG_H
# include "../config.h"
#endif // HAVE_CONFIG_H

#include "i18n.h"
#include "Configmenu.h"
#include "Toolbar.h"
#include "Window.h"
#include "Screen.h"

Configmenu::Configmenu(BScreen &scr) : Basemenu(scr), screen(scr)
{
  setLabel(i18n(ConfigmenuSet, ConfigmenuConfigOptions,
                "Config options"));
  setInternalMenu();

  focusmenu = new Focusmenu(this);
  placementmenu = new Placementmenu(this);

  insert(i18n(ConfigmenuSet, ConfigmenuFocusModel,
              "Focus Model"), focusmenu);
  insert(i18n(ConfigmenuSet, ConfigmenuWindowPlacement,
              "Window Placement"), placementmenu);
  insert(i18n(ConfigmenuSet, ConfigmenuImageDithering,
              "Image Dithering"), 1);
  insert(i18n(ConfigmenuSet, ConfigmenuOpaqueMove,
              "Opaque Window Moving"), 2);
  insert(i18n(ConfigmenuSet, ConfigmenuFullMax,
              "Full Maximization"), 3);
  insert(i18n(ConfigmenuSet, ConfigmenuFocusNew,
              "Focus New Windows"), 4);
  insert(i18n(ConfigmenuSet, ConfigmenuFocusLast,
              "Focus Last Window on Workspace"), 5);
  insert(i18n(ConfigmenuSet, ConfigmenuHideToolbar,
              "Hide toolbar"), 6);
  update();

  setValues();
}

void Configmenu::setValues() {
  setItemSelected(2, screen.imageDither());
  setItemSelected(3, screen.opaqueMove());
  setItemSelected(4, screen.fullMax());
  setItemSelected(5, screen.focusNew());
  setItemSelected(6, screen.focusLast());
  setItemSelected(7, screen.hideToolbar());
}

Configmenu::~Configmenu() {
  delete focusmenu;
  delete placementmenu;
}

void Configmenu::itemSelected(int button, int index) {
  if (button != 1)
    return;

  BasemenuItem *item = find(index);

  if (!item->function())
    return;

  switch(item->function()) {
  case 1: { // dither
    screen.setImageDither(!screen.imageDither());

    setItemSelected(index, screen.imageDither());

    break;
  }

  case 2: { // opaque move
    screen.setOpaqueMove(!screen.opaqueMove());

    setItemSelected(index, screen.opaqueMove());

    break;
  }

  case 3: { // full maximization
    screen.setFullMax(!screen.fullMax());

    setItemSelected(index, screen.fullMax());

    break;
  }
  case 4: { // focus new windows
    screen.setFocusNew(!screen.focusNew());

    setItemSelected(index, screen.focusNew());
    break;
  }

  case 5: { // focus last window on workspace
    screen.setFocusLast(!screen.focusLast());
    setItemSelected(index, screen.focusLast());
    break;
  }
  case 6:{ //toggle toolbar hide
    screen.setHideToolbar(!screen.hideToolbar());
    setItemSelected(index, screen.hideToolbar());
    break;
  }
  } // switch
}

void Configmenu::reconfigure() {
  setValues();
  focusmenu->reconfigure();
  placementmenu->reconfigure();

  Basemenu::reconfigure();
}

Configmenu::Focusmenu::Focusmenu(Configmenu *cm) : Basemenu(cm->screen) {
  configmenu = cm;

  setLabel(i18n(ConfigmenuSet, ConfigmenuFocusModel,
                "Focus Model"));
  setInternalMenu();

  insert(i18n(ConfigmenuSet, ConfigmenuClickToFocus,
              "Click To Focus"), 1);
  insert(i18n(ConfigmenuSet, ConfigmenuSloppyFocus,
              "Sloppy Focus"), 2);
  insert(i18n(ConfigmenuSet, ConfigmenuAutoRaise,
              "Auto Raise"), 3);
  update();

  setValues();
}

void Configmenu::Focusmenu::setValues() {
  setItemSelected(0, !configmenu->screen.sloppyFocus());
  setItemSelected(1, configmenu->screen.sloppyFocus());
  setItemEnabled(2, configmenu->screen.sloppyFocus());
  setItemSelected(2, configmenu->screen.autoRaise());
}

void Configmenu::Focusmenu::reconfigure() {
  setValues();
  Basemenu::reconfigure();
}

void Configmenu::Focusmenu::itemSelected(int button, int index) {
  if (button != 1)
    return;

  BasemenuItem *item = find(index);

  if (!item->function())
    return;

  switch (item->function()) {
  case 1: // click to focus
    configmenu->screen.setSloppyFocus(false);
    configmenu->screen.setAutoRaise(false);
    // make windows all grab button1 clicks
    configmenu->screen.reconfigure();
    break;

  case 2: // sloppy focus
    configmenu->screen.setSloppyFocus(true);
    // make windows stop grabbing button1 clicks
    configmenu->screen.reconfigure();
    break;

  case 3: // auto raise with sloppy focus
    configmenu->screen.setAutoRaise(!configmenu->screen.autoRaise());
    break;
  }

  setItemSelected(0, !configmenu->screen.sloppyFocus());
  setItemSelected(1, configmenu->screen.sloppyFocus());
  setItemEnabled(2, configmenu->screen.sloppyFocus());
  setItemSelected(2, configmenu->screen.autoRaise());
}

Configmenu::Placementmenu::Placementmenu(Configmenu *cm) :
Basemenu(cm->screen) {
  configmenu = cm;

  setLabel(i18n(ConfigmenuSet, ConfigmenuWindowPlacement,
                "Window Placement"));
  setInternalMenu();

  insert(i18n(ConfigmenuSet, ConfigmenuSmartRows,
              "Smart Placement (Rows)"),
         BScreen::RowSmartPlacement);
  insert(i18n(ConfigmenuSet, ConfigmenuSmartCols,
              "Smart Placement (Columns)"),
         BScreen::ColSmartPlacement);
  insert(i18n(ConfigmenuSet, ConfigmenuCascade,
              "Cascade Placement"), BScreen::CascadePlacement);
  insert(i18n(ConfigmenuSet, ConfigmenuBestFit,
              "Best Fit Placement"), BScreen::BestFitPlacement);
  insert(i18n(ConfigmenuSet, ConfigmenuUnderMouse,
              "Under Mouse Placement"),
         BScreen::UnderMousePlacement);
  insert(i18n(ConfigmenuSet, ConfigmenuClickMouse,
              "Click Mouse Placement"),
         BScreen::ClickMousePlacement);
  insert(i18n(ConfigmenuSet, ConfigmenuLeftRight,
              "Left to Right"), BScreen::LeftRight);
  insert(i18n(ConfigmenuSet, ConfigmenuRightLeft,
              "Right to Left"), BScreen::RightLeft);
  insert(i18n(ConfigmenuSet, ConfigmenuTopBottom,
              "Top to Bottom"), BScreen::TopBottom);
  insert(i18n(ConfigmenuSet, ConfigmenuBottomTop,
              "Bottom to Top"), BScreen::BottomTop);
  update();

  setValues();
}

void Configmenu::Placementmenu::setValues() {
  const int p = configmenu->screen.placementPolicy();
  setItemSelected(0, p == BScreen::RowSmartPlacement);
  setItemSelected(1, p == BScreen::ColSmartPlacement);
  setItemSelected(2, p == BScreen::CascadePlacement);
  setItemSelected(3, p == BScreen::BestFitPlacement);
  setItemSelected(4, p == BScreen::UnderMousePlacement);
  setItemSelected(5, p == BScreen::ClickMousePlacement);

  bool rl = (configmenu->screen.rowPlacementDirection() ==
             BScreen::LeftRight),
  tb = (configmenu->screen.colPlacementDirection() ==
        BScreen::TopBottom);

  setItemSelected(6, rl);
  setItemEnabled(6, (p != BScreen::UnderMousePlacement &&
                     p != BScreen::ClickMousePlacement));
  setItemSelected(7, !rl);
  setItemEnabled(7, (p != BScreen::UnderMousePlacement &&
                     p != BScreen::ClickMousePlacement));

  setItemSelected(8, tb);
  setItemEnabled(8, (p != BScreen::UnderMousePlacement &&
                     p != BScreen::ClickMousePlacement));
  setItemSelected(9, !tb);
  setItemEnabled(9, (p != BScreen::UnderMousePlacement &&
                     p != BScreen::ClickMousePlacement));
}

void Configmenu::Placementmenu::reconfigure() {
  setValues();
  Basemenu::reconfigure();
}

void Configmenu::Placementmenu::itemSelected(int button, int index) {
  if (button != 1)
    return;

  BasemenuItem *item = find(index);

  if (!item->function())
    return;

  switch (item->function()) {
  case BScreen::RowSmartPlacement:
    configmenu->screen.setPlacementPolicy(item->function());
    break;

  case BScreen::ColSmartPlacement:
    configmenu->screen.setPlacementPolicy(item->function());
    break;

  case BScreen::CascadePlacement:
    configmenu->screen.setPlacementPolicy(item->function());
    break;

  case BScreen::BestFitPlacement:
    configmenu->screen.setPlacementPolicy(item->function());
    break;

  case BScreen::UnderMousePlacement:
    configmenu->screen.setPlacementPolicy(item->function());
    break;

  case BScreen::ClickMousePlacement:
    configmenu->screen.setPlacementPolicy(item->function());
    break;

  case BScreen::LeftRight:
    configmenu->screen.setRowPlacementDirection(BScreen::LeftRight);
    break;

  case BScreen::RightLeft:
    configmenu->screen.setRowPlacementDirection(BScreen::RightLeft);
    break;

  case BScreen::TopBottom:
    configmenu->screen.setColPlacementDirection(BScreen::TopBottom);
    break;

  case BScreen::BottomTop:
    configmenu->screen.setColPlacementDirection(BScreen::BottomTop);
    break;
  }
  setValues();
}
