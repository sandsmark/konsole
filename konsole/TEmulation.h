/*
    This file is part of Konsole, an X terminal.
    Copyright (C) 1997,1998 by Lars Doelle <lars.doelle@on-line.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301  USA.
*/

#ifndef EMULATION_H
#define EMULATION_H

#include "TEWidget.h"
#include "TEScreen.h"
#include <QTimer>
//Added by qt3to4:
#include <QTextStream>
#include <QKeyEvent>
#include <stdio.h>
#include <QTextCodec>
#include <QPointer>
#include <keytrans.h>

enum { NOTIFYNORMAL=0, NOTIFYBELL=1, NOTIFYACTIVITY=2, NOTIFYSILENCE=3 };

class TEmulation : public QObject
{ Q_OBJECT

public:

  TEmulation(TEWidget* gui);
  ~TEmulation();

  virtual void changeGUI(TEWidget* newgui);

  /** 
   * Adds a new view for this emulation.
   *
   * When the emulation output changes, the view will be updated to display the new output.
   */
  virtual void addView(TEWidget* widget);
  /**
   * Removes a view from this emulation.
   *
   * @p widget will no longer be updated when the emulation output changes.
   */
  virtual void removeView(TEWidget* widget);

public:
  QSize imageSize();
  virtual void setHistory(const HistoryType&);
  const QTextCodec *codec() { return m_codec; }
  void setCodec(const QTextCodec *);
  virtual const HistoryType& history();
  virtual void streamHistory(QTextStream*,TerminalCharacterDecoder* decoder);

  virtual void findTextBegin();
  virtual bool findTextNext( const QString &str, bool forward, bool caseSensitive, bool regExp );

public Q_SLOTS: // signals incoming from TEWidget

  virtual void onImageSizeChange(int lines, int columns);
  virtual void onHistoryCursorChange(int cursor);
  virtual void onKeyPress(QKeyEvent*);
 
  virtual void clearSelection();
  virtual void copySelection();
  virtual void onSelectionBegin(const int x, const int y, const bool columnmode);
  virtual void onSelectionExtend(const int x, const int y);
  virtual void setSelection(const bool preserve_line_breaks);
  virtual void isBusySelecting(bool busy);
  virtual void testIsSelected(const int x, const int y, bool &selected);

public Q_SLOTS: // signals incoming from data source

  void onRcvBlock(const char* txt,int len);

Q_SIGNALS:

  void lockPty(bool);
  void useUtf8(bool);
  void sndBlock(const char* txt,int len);
  void ImageSizeChanged(int lines, int columns);
  void changeColumns(int columns);
  void changeColLin(int columns, int lines);
  void changeTitle(int arg, const char* str);
  void notifySessionState(int state);
  void zmodemDetected();
  void changeTabTextColor(int color);

public:

  virtual void onRcvChar(int);

  virtual void setMode  (int) = 0;
  virtual void resetMode(int) = 0;

  virtual void sendString(const char*) = 0;

  virtual void setConnect(bool r);
  bool isConnected() { return connected; }
  
  bool utf8() { return m_codec->mibEnum() == 106; }

  virtual char getErase();

  virtual void setListenToKeyPress(bool l);
  void setColumns(int columns);

  void setKeymap(int no);
  void setKeymap(const QString &id);
  int keymapNo();
  QString keymap();

  virtual void clearEntireScreen() =0;
  virtual void reset() =0;

protected:

  QList< QPointer<TEWidget> > _views;

  //QPointer<TEWidget> gui;
  
  TEScreen* scr;         // referes to one `screen'
  TEScreen* screen[2];   // 0 = primary, 1 = alternate
  void setScreen(int n); // set `scr' to `screen[n]'

  bool   connected;    // communicate with widget
  bool   listenToKeyPress;  // listen to input

  void setCodec(int c); // codec number, 0 = locale, 1=utf8

  //decodes an incoming C-style character stream into a unicode QString using 
  //the current text codec.  (this allows for rendering of non-ASCII characters in text files etc.)
  const QTextCodec* m_codec;
  QTextDecoder* decoder;

  KeyTrans* keytrans;

// refreshing related material.
// this is localized in the class.
private Q_SLOTS: // triggered by timer

  void showBulk();

private:

  void connectView(TEWidget* widget);
  //void connectGUI();

  void bulkStart();

private:

  QTimer bulk_timer1;
  QTimer bulk_timer2;
  
  int    m_findPos;
};

#endif // ifndef EMULATION_H
