#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include "main.h"

//  ######     ###    ##    ## ##     ##    ###     ######
// ##    ##   ## ##   ###   ## ##     ##   ## ##   ##    ##
// ##        ##   ##  ####  ## ##     ##  ##   ##  ##
// ##       ##     ## ## ## ## ##     ## ##     ##  ######
// ##       ######### ##  ####  ##   ##  #########       ##
// ##    ## ##     ## ##   ###   ## ##   ##     ## ##    ##
//  ######  ##     ## ##    ##    ###    ##     ##  ######

// called whenever size, position, stacking of widget window changes
gboolean canvas_configure_cb(   GtkWidget*          widget,
                                GdkEventConfigure*  event,
                                sim_t*              sim ) {
    if (sim->surface)
        cairo_surface_destroy( sim->surface );

    // inherit surface size from parent
    sim->surface = gdk_window_create_similar_surface(
                                    gtk_widget_get_window( widget ),
                                    CAIRO_CONTENT_COLOR,
                                    gtk_widget_get_allocated_width( widget ),
                                    gtk_widget_get_allocated_height( widget ));

    // draw surface
    draw_env( widget, sim );

    /* We've handled the configure event, no need for further processing. */
    return TRUE;
}

gboolean canvas_mouse_motion_cb(    GtkWidget*      widget,
                                    GdkEventMotion* event,
                                    sim_t*          sim ) {
    /* paranoia check, in case we haven't gotten a configure event */
    if (sim->surface == NULL)
        return FALSE;

    if (event->state & GDK_BUTTON1_MASK) {
        //randomizeArray2DInt(sim->env, sim->lenX, sim->lenY, 50, 25, 25);
        sim_run( sim );
        draw_env( widget, sim );
    }

    /* We've handled it, stop processing */
    return TRUE;
}

gboolean canvas_mouse_press_cb( GtkWidget*      widget,
                                GdkEventButton* event,
                                sim_t*          sim ) {
    /* paranoia check, in case we haven't gotten a configure event */
    if (sim->surface == NULL)
        return FALSE;
    if (event->button == GDK_BUTTON_PRIMARY) {
        //randomizeArray2DInt(sim->env, sim->lenX, sim->lenY, 25, 25, 50);
        draw_env( widget, sim );
    }
    else if (event->button == GDK_BUTTON_SECONDARY) {
        //randomizeArray2DInt(sim->env, sim->lenX, sim->lenY, 25, 25, 50);
        draw_env( widget, sim );
    }
    return TRUE;
}

gboolean b_initialize_cb(  GtkWidget* widget,
                    GdkEventMotion* event,
                    sim_t* sim ) {
    // wrapper for running the initialization for the sim
    sim_init( sim );
    // sim->drawingArea is defined at app initialization, so it should
        // exist by the time this is called
    window_set_rate_sliders( sim );
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(sim->ui.buttonToggle), FALSE);
    draw_env (sim->ui.drawingArea, sim);

    return TRUE;
}

gboolean b_run_toggle_cb(   GtkWidget* widget,
                            sim_t* sim ) {
    sim->active = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON (widget) );
    if (gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(widget) )) {
        gtk_button_set_label( GTK_BUTTON(widget), "Running" );
    }
    else {
        gtk_button_set_label( GTK_BUTTON(widget), "Paused" );
    }
    return TRUE;
}

gboolean s_sliders_cb(  GtkRange*       widget,
                        GtkScrollType   scroll,
                        gdouble         value,
                        sim_t*          sim) {
    int i;

    for (i=0; i<sim->nReactions; i++) {
        if (GTK_RANGE(widget) == GTK_RANGE(sim->ui.listSliders[i])) {
            //printf("%d\n", i);
            sim->weightMatrix[sim->reactionList[i].source][sim->reactionList[i].sink]
                = value;
            break;
        }
    }
    // the sliding also needs to be handled by GTK's default handler, so
        // allow it to propagate further
    return FALSE;
}

void window_set_rate_sliders( sim_t* sim ) {
    // if sliders exist, destroy them; this is done because the number of
        // sliders needed now might be different, and we don't know the
        // previous value
    if (sim->ui.gridSliders != NULL) {
        // destroy grid widget; this should also destroy all its children
            // ie the sliders contained within grid
        gtk_widget_destroy( sim->ui.gridSliders );
        deallocateWidgetList( sim->ui.listSliders );
        sim->ui.gridSliders = NULL;
        sim->ui.listSliders = NULL;
    }

    int i;

    // create container
    sim->ui.gridSliders = gtk_grid_new();
    gtk_container_add( GTK_CONTAINER(sim->ui.expanderSliders), sim->ui.gridSliders );
    // create slider array
    sim->ui.listSliders = allocateWidgetList( sim->nReactions );

    for (i=0; i<sim->nReactions; i++) {
        sim->ui.listSliders[i] = gtk_scale_new_with_range(
                    GTK_ORIENTATION_HORIZONTAL, 0.0, 50.0, 0.01 );
        gtk_scale_set_draw_value( GTK_SCALE(sim->ui.listSliders[i]), TRUE );
        gtk_range_set_value( GTK_RANGE(sim->ui.listSliders[i]),
                    sim->weightMatrix[sim->reactionList[i].source][sim->reactionList[i].sink] );
        g_signal_connect( sim->ui.listSliders[i], "change-value",
                    G_CALLBACK(s_sliders_cb), sim );
        gtk_grid_attach( GTK_GRID(sim->ui.gridSliders), sim->ui.listSliders[i], 0, i, 2, 1 );

        // do not expand on window resize
        gtk_widget_set_size_request( sim->ui.listSliders[i], 150, 10 );
        gtk_widget_set_hexpand( sim->ui.listSliders[i], FALSE );
        gtk_widget_set_halign( sim->ui.listSliders[i], GTK_ALIGN_START );
        gtk_widget_set_vexpand( sim->ui.listSliders[i], FALSE );
        gtk_widget_set_valign( sim->ui.listSliders[i], GTK_ALIGN_CENTER );
    }
    gtk_widget_show_all( sim->ui.gridSliders );
    return;
}


void close_window( sim_t* sim ) {
    if (sim->surface)
        cairo_surface_destroy( sim->surface );
}

/* Redraw the screen from the surface. Note that the ::draw
 * signal receives a ready-to-be-used cairo_t that is already
 * clipped to only draw the exposed areas of the widget
 */
gboolean canvas_draw_cb(    GtkWidget*  widget,
                            cairo_t*    cr,
                            sim_t*      sim ) {
    if (sim->surface == NULL)
        return FALSE;
    cairo_set_source_surface( cr, sim->surface, 0, 0 );
    cairo_paint( cr );

    return FALSE;
}

void app_init(  GtkApplication* app,
                sim_t*          sim ) {
    GtkWidget* window;
    GtkWidget* g_top;   // grid
    GtkWidget* b_initialize;
    GtkWidget* b_run_toggle;
    GtkWidget* f_drawing_area;
    GtkWidget* drawing_area;
    GtkWidget* sc_sliders;  // scrollable container
    GtkWidget* e_sliders;   // expander

    // layout:
    /*
    window
    ├-- stack
        ├-- b_initialize
        ├-- b_run_toggle
        ├-- f_drawing_area
            ├-- drawing_area
        ├-- sc_sliders
            ├-- e_sliders
                ├-- g_sliders
                    ├-- array of sliders
    */

    // create window
    window = gtk_application_window_new( app );
    gtk_window_set_title( GTK_WINDOW(window), "SLLVM Simulation Environment" );
    gtk_container_set_border_width( GTK_CONTAINER (window), 8 );

    // set up close signal for cleanly exiting app
    g_signal_connect( window, "destroy", G_CALLBACK(close_window), sim );



    // set up top level grid and pack into window
    g_top = gtk_grid_new();
    gtk_container_add( GTK_CONTAINER(window), g_top );



    // create frame for simulation environment and pack into grid
    f_drawing_area = gtk_frame_new( "Sim" );
    gtk_frame_set_shadow_type( GTK_FRAME(f_drawing_area), GTK_SHADOW_IN );
    gtk_grid_attach( GTK_GRID(g_top), f_drawing_area, 0, 1, 5, 5 );

    // create simulation environment and pack into the frame
    drawing_area = gtk_drawing_area_new ();
    sim->ui.drawingArea = drawing_area;
    // set minimum size
    gtk_widget_set_size_request( drawing_area, 300, 300 );
    // set to fill excess of container
    gtk_widget_set_hexpand( drawing_area, TRUE );
    gtk_widget_set_halign( drawing_area, GTK_ALIGN_FILL );
    gtk_widget_set_vexpand( drawing_area, TRUE );
    gtk_widget_set_valign( drawing_area, GTK_ALIGN_FILL );
    // pack into frame
    gtk_container_add( GTK_CONTAINER(f_drawing_area), drawing_area );



    // create container for rate sliders
    sc_sliders = gtk_scrolled_window_new( NULL, NULL );
    gtk_widget_set_size_request( sc_sliders, 150, 10 );
    gtk_grid_attach( GTK_GRID(g_top), sc_sliders, 6, 1, 1, 5 );
    e_sliders = gtk_expander_new( "Rates" );
    sim->ui.expanderSliders = e_sliders;
    gtk_container_add( GTK_CONTAINER(sc_sliders), e_sliders );





    // create control buttons and pack into grid
    b_initialize = gtk_button_new_with_label( "Initialize" );
    sim->ui.buttonInitialize = b_initialize;
    g_signal_connect( b_initialize, "clicked",
                    G_CALLBACK(b_initialize_cb), sim );
    gtk_grid_attach( GTK_GRID(g_top), b_initialize, 0, 0, 1, 1 );

    b_run_toggle = gtk_toggle_button_new_with_label( "Paused" );
    sim->ui.buttonToggle = b_run_toggle;
    g_signal_connect( b_run_toggle, "toggled",
                    G_CALLBACK(b_run_toggle_cb), sim );
    gtk_grid_attach( GTK_GRID(g_top), b_run_toggle, 1, 0, 1, 1 );



    // setup signals
    // draw is called whenever the widget renders itself
    g_signal_connect( drawing_area, "draw",
                    G_CALLBACK(canvas_draw_cb), sim );
    // configure-event is called when size, position, stacking of widget window
        // changes
    g_signal_connect( drawing_area,"configure-event",
                    G_CALLBACK(canvas_configure_cb), sim );

    // set up user control signals

    /* Event signals */
    g_signal_connect( drawing_area, "motion-notify-event",
                    G_CALLBACK(canvas_mouse_motion_cb), sim );
    g_signal_connect( drawing_area, "button-press-event",
                    G_CALLBACK(canvas_mouse_press_cb), sim );

    /* Ask to receive events the drawing area doesn't normally
    * subscribe to. In particular, we need to ask for the
    * button press and motion notify events that want to handle.
    */
    gtk_widget_set_events( drawing_area, gtk_widget_get_events(drawing_area)
                         | GDK_BUTTON_PRESS_MASK
                         | GDK_POINTER_MOTION_MASK );

    // add main loop to run in background
    g_idle_add( sim_loop, (gpointer)sim );

    gtk_widget_show_all( window );
}
