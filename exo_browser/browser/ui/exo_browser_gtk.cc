// Copyright (c) 2013 Stanislas Polu.
// See the LICENSE file.


#include "exo/exo_browser/browser/ui/exo_browser.h"

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_view.h"
#include "exo/exo_browser/browser/ui/exo_frame.h"

using namespace content;

namespace exo_browser {

void 
ExoBrowser::PlatformInitialize(
    const gfx::Size& default_window_size) 
{
}

void 
ExoBrowser::PlatformCleanUp() 
{
  /* Nothing to clean up; GTK will clean up the widgets shortly after. */
}

void 
ExoBrowser::PlatformCreateWindow(
    int width,
    int height)
{
  window_ = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
  gtk_window_set_title(window_, "Exo");

  fixed_ = gtk_fixed_new();
  hbox_ = gtk_hbox_new(FALSE, 0);
  vbox_ = gtk_vbox_new(FALSE, 0);

  control_left_box_ = gtk_event_box_new();
  control_right_box_ = gtk_event_box_new();
  control_top_box_ = gtk_event_box_new();
  control_bottom_box_ = gtk_event_box_new();

  control_hover_left_box_ = gtk_event_box_new();
  control_hover_right_box_ = gtk_event_box_new();
  control_hover_top_box_ = gtk_event_box_new();
  control_hover_bottom_box_ = gtk_event_box_new();

  gtk_widget_set_size_request(control_left_box_, 0, 0);
  gtk_widget_set_size_request(control_right_box_, 0, 0);
  gtk_widget_set_size_request(control_top_box_, 0, 0);
  gtk_widget_set_size_request(control_bottom_box_, 0, 0);
  
  pages_box_ = gtk_event_box_new();
  visible_page_ = NULL;

  gtk_box_pack_start(GTK_BOX(hbox_), control_left_box_, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox_), vbox_, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox_), control_right_box_, FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(vbox_), control_top_box_, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox_), pages_box_, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox_), control_bottom_box_, FALSE, FALSE, 0);

  /* They will get repositioned/resized at the next window resize */
  gtk_fixed_put(GTK_FIXED(fixed_), control_hover_left_box_, 0, 0);
  gtk_fixed_put(GTK_FIXED(fixed_), control_hover_right_box_, 0, 0);
  gtk_fixed_put(GTK_FIXED(fixed_), control_hover_top_box_, 0, 0);
  gtk_fixed_put(GTK_FIXED(fixed_), control_hover_bottom_box_, 0, 0);

  gtk_fixed_put(GTK_FIXED(fixed_), hbox_, 0, 0);

  g_signal_connect(G_OBJECT(window_), "destroy",
                   G_CALLBACK(OnWindowDestroyedThunk), this);
  g_signal_connect(G_OBJECT(window_), "check-resize",
                   G_CALLBACK(OnWindowCheckResizeThunk), this);

  gtk_container_add(GTK_CONTAINER(window_), fixed_);

  gtk_window_resize(window_, width, height);

  // Finally, show the window.
  gtk_widget_show_all(GTK_WIDGET(window_));
}

void 
ExoBrowser::PlatformKill() 
{
  gtk_widget_destroy(GTK_WIDGET(window_));
}

void 
ExoBrowser::PlatformSetTitle(
    const std::string& title) 
{
  gtk_window_set_title(GTK_WINDOW(window_), title.c_str());
}



void 
ExoBrowser::PlatformAddPage(
    ExoFrame *frame)
{
  /* Nothing to Do? */
}


void 
ExoBrowser::PlatformRemovePage(
    ExoFrame *frame)
{
  WebContentsView* content_view = frame->web_contents_->GetView();
  if(visible_page_ == content_view->GetNativeView()) {
    gtk_container_remove(GTK_CONTAINER(pages_box_), visible_page_);
    visible_page_ = NULL;
  }
}


void 
ExoBrowser::PlatformShowPage(
    ExoFrame *frame)
{
  WebContentsView* content_view = frame->web_contents_->GetView();
  if(visible_page_ != content_view->GetNativeView()) {
    if(visible_page_ != NULL) {
      gtk_container_remove(GTK_CONTAINER(pages_box_), visible_page_);
    }
    visible_page_ = content_view->GetNativeView();
    gtk_container_add(GTK_CONTAINER(pages_box_), visible_page_);
  }
}


void 
ExoBrowser::PlatformSetControl(
    CONTROL_TYPE type, 
    ExoFrame *frame)
{
  int w, h;
  gtk_window_get_size(window_, &w, &h);
  WebContentsView* content_view = frame->web_contents_->GetView();

  switch(type) {
    case LEFT_CONTROL: 
      gtk_container_add(GTK_CONTAINER(control_left_box_),
                        content_view->GetNativeView());
      gtk_widget_set_size_request(control_left_box_, 0, 0);
      break;
    case RIGHT_CONTROL: 
      gtk_container_add(GTK_CONTAINER(control_right_box_),
                        content_view->GetNativeView());
      gtk_widget_set_size_request(control_right_box_, 0, 0);
      break;
    case TOP_CONTROL: 
      gtk_container_add(GTK_CONTAINER(control_top_box_),
                        content_view->GetNativeView());
      gtk_widget_set_size_request(control_top_box_, 0, 0);
      break;
    case BOTTOM_CONTROL: 
      gtk_container_add(GTK_CONTAINER(control_bottom_box_),
                        content_view->GetNativeView());
      gtk_widget_set_size_request(control_bottom_box_, 0, 0);
      break;
    case HOVER_LEFT_CONTROL:
      gtk_container_add(GTK_CONTAINER(control_hover_left_box_),
                        content_view->GetNativeView());
      gtk_widget_set_size_request(control_hover_left_box_, 0, w_height_);
    case HOVER_RIGHT_CONTROL:
      gtk_container_add(GTK_CONTAINER(control_hover_right_box_),
                        content_view->GetNativeView());
      gtk_widget_set_size_request(control_hover_right_box_, 0, w_height_);
    case HOVER_TOP_CONTROL:
      gtk_container_add(GTK_CONTAINER(control_hover_top_box_),
                        content_view->GetNativeView());
      gtk_widget_set_size_request(control_hover_top_box_, w_width_, 0);
    case HOVER_BOTTOM_CONTROL:
      gtk_container_add(GTK_CONTAINER(control_hover_bottom_box_),
                        content_view->GetNativeView());
      gtk_widget_set_size_request(control_hover_bottom_box_, w_width_, 0);
    default:
      /* Nothing to do */
      ;
  }
}


void 
ExoBrowser::PlatformSetControlDimension(
    CONTROL_TYPE type, 
    int size)
{
  int w, h;
  gtk_window_get_size(window_, &w, &h);

  switch(type) {
    case LEFT_CONTROL: 
      gtk_widget_set_size_request(control_left_box_, size, 0);
      break;
    case RIGHT_CONTROL: 
      gtk_widget_set_size_request(control_right_box_, size, 0);
      break;
    case TOP_CONTROL: 
      gtk_widget_set_size_request(control_top_box_, 0, size);
      break;
    case BOTTOM_CONTROL: 
      gtk_widget_set_size_request(control_bottom_box_, 0, size);
      break;
    case HOVER_LEFT_CONTROL: 
      gtk_widget_set_size_request(control_hover_left_box_, size, w_height_);
      break;
    case HOVER_RIGHT_CONTROL: 
      gtk_widget_set_size_request(control_hover_right_box_, size, w_height_);
      break;
    case HOVER_TOP_CONTROL: 
      gtk_widget_set_size_request(control_hover_top_box_, w_width_, size);
      break;
    case HOVER_BOTTOM_CONTROL: 
      gtk_widget_set_size_request(control_hover_bottom_box_, w_width_, size);
      break;
    default:
      /* Nothing to do */
      ;
  }
}


void 
ExoBrowser::PlatformUnsetControl(
    CONTROL_TYPE type, 
    ExoFrame *frame)
{
  WebContentsView* content_view = frame->web_contents_->GetView();

  switch(type) {
    case LEFT_CONTROL: 
      gtk_container_remove(GTK_CONTAINER(control_left_box_),
                           content_view->GetNativeView());
      gtk_widget_set_size_request(control_left_box_, 0, 0);
      break;
    case RIGHT_CONTROL: 
      gtk_container_remove(GTK_CONTAINER(control_right_box_),
                           content_view->GetNativeView());
      gtk_widget_set_size_request(control_right_box_, 0, 0);
      break;
    case TOP_CONTROL: 
      gtk_container_remove(GTK_CONTAINER(control_top_box_),
                           content_view->GetNativeView());
      gtk_widget_set_size_request(control_top_box_, 0, 0);
      break;
    case BOTTOM_CONTROL: 
      gtk_container_remove(GTK_CONTAINER(control_bottom_box_),
                           content_view->GetNativeView());
      gtk_widget_set_size_request(control_bottom_box_, 0, 0);
      break;
    case HOVER_LEFT_CONTROL: 
      gtk_container_remove(GTK_CONTAINER(control_hover_left_box_),
                           content_view->GetNativeView());
      gtk_widget_set_size_request(control_hover_left_box_, 0, w_height_);
      break;
    case HOVER_RIGHT_CONTROL: 
      gtk_container_remove(GTK_CONTAINER(control_hover_right_box_),
                           content_view->GetNativeView());
      gtk_widget_set_size_request(control_hover_right_box_, 0, w_height_);
      break;
    case HOVER_TOP_CONTROL: 
      gtk_container_remove(GTK_CONTAINER(control_hover_top_box_),
                           content_view->GetNativeView());
      gtk_widget_set_size_request(control_hover_top_box_, w_width_, 0);
      break;
    case HOVER_BOTTOM_CONTROL: 
      gtk_container_remove(GTK_CONTAINER(control_hover_bottom_box_),
                           content_view->GetNativeView());
      gtk_widget_set_size_request(control_hover_bottom_box_, w_width_, 0);
      break;
    default:
      /* Nothing to do */
      ;
  }
}


void
ExoBrowser::PlatformFocus()
{
  gtk_window_present(window_);
}

void
ExoBrowser::PlatformMaximize()
{
  gtk_window_maximize(window_);
}


gfx::Size
ExoBrowser::PlatformSize()
{
  int w,h;
  gtk_window_get_size(window_, &w, &h);
  return gfx::Size(w, h);
}

gfx::Point
ExoBrowser::PlatformPosition()
{
  int x,y;
  gtk_window_get_position(window_, &x, &y);
  return gfx::Point(x, y);
}

gboolean 
ExoBrowser::OnWindowCheckResize(
    GtkWidget* window) 
{
  int w, h;
  gtk_window_get_size(window_, &w, &h);

  if(w_width_ != w || w_height_ != h) {
    w_width_ = w;
    w_height_ = h;

    LOG(INFO) << "WINDOW DO RESIZE: " << w << " " << h;

    /*
    gtk_widget_set_size_request(fixed_, w, h);
    */
    /*
    gtk_widget_set_size_request(hbox_, w, h);
    */
  
    /*
    gtk_fixed_move(GTK_FIXED(fixed_), control_hover_left_box_, 
                   0, 0);
    gtk_widget_set_size_request(control_hover_left_box_, 
                                dimensions_[HOVER_LEFT_CONTROL], h);
    gtk_fixed_move(GTK_FIXED(fixed_), control_hover_right_box_, 
                   w - dimensions_[HOVER_RIGHT_CONTROL], 0);
    gtk_widget_set_size_request(control_hover_right_box_, 
                                dimensions_[HOVER_RIGHT_CONTROL], h);
    gtk_fixed_move(GTK_FIXED(fixed_), control_hover_top_box_, 
                   0, 0);
    gtk_widget_set_size_request(control_hover_top_box_, 
                                w, dimensions_[HOVER_TOP_CONTROL]);
    gtk_fixed_move(GTK_FIXED(fixed_), control_hover_bottom_box_, 
                   0, h - dimensions_[HOVER_BOTTOM_CONTROL]);
    gtk_widget_set_size_request(control_hover_top_box_, 
                                w, dimensions_[HOVER_BOTTOM_CONTROL]);
    */
  }
  
  return FALSE;  // Don't stop this message.
}

gboolean 
ExoBrowser::OnWindowDestroyed(
    GtkWidget* window) 
{
  LOG(INFO) << "WINDOW DESTROYED";
  /* We call Kill which will dispatch an event to the API as we don't expect */
  /* any other behavior here. If the ExoBrowser has not been already killed. */
  if(!is_killed_)
    Kill();
  return FALSE;  // Don't stop this message.
}

} // namespace exo_browser
