#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include "main.h"


//  ######  ##       ########    ###    ########
// ##    ## ##       ##         ## ##   ##     ##
// ##       ##       ##        ##   ##  ##     ##
// ##       ##       ######   ##     ## ########
// ##       ##       ##       ######### ##   ##
// ##    ## ##       ##       ##     ## ##    ##
//  ######  ######## ######## ##     ## ##     ##

void draw_clear( sim_t* sim ) {
    cairo_t* cr;

    cr = cairo_create (sim->surface);

    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_paint (cr);

    cairo_destroy (cr);
}

// ######## ##    ## ##     ##
// ##       ###   ## ##     ##
// ##       ####  ## ##     ##
// ######   ## ## ## ##     ##
// ##       ##  ####  ##   ##
// ##       ##   ###   ## ##
// ######## ##    ##    ###

void draw_env( GtkWidget* widget, sim_t* sim ) {

    // if env does not exist yet, draw a blank screen
    if (sim->env == NULL) {
        draw_clear( sim );
        return;
    }


    cairo_t* cr;
    int i,j;
    // pixel buffers
    GdkPixbuf* pixbuf;
    GdkPixbuf* rpixbuf;
    // resizing variables
    int widgetWidth, widgetHeight, newWidth, newHeight;
    double scaleFactor;

    // paint to surface
    cr = cairo_create(sim->surface);

    // first create the pixelbuffer
    pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, sim->lenX, sim->lenY);
    // fill background with black (FIX)
    gdk_pixbuf_fill(pixbuf, 0xffffff);
    // save variables for easier use in loop
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    guchar* pixels = gdk_pixbuf_get_pixels(pixbuf);

    // loop through every array entry and save the given color in the pixbuf
    // pixbuf is a 1D array of unsigned chars (int) that represents the rgb
        // components; each "row" in the abstracted 2D array has a length of
        // rowstride number of ints, which roughly corresponds to
        // width*#channels (here 3 for rgb) plus a small buffer to align the
        // rows
    for(i=0; i<sim->lenX; i++) {
        for(j=0; j<sim->lenY; j++) {
            pixels[(j*rowstride + i*3) + 0] = sim->channelR[sim->env[i][j]];
            pixels[(j*rowstride + i*3) + 1] = sim->channelG[sim->env[i][j]];
            pixels[(j*rowstride + i*3) + 2] = sim->channelB[sim->env[i][j]];
        }
    }

    // calculate a single scale factor that fills the space as much as
        // possible but without stretching
    widgetWidth = gtk_widget_get_allocated_width(widget);
    widgetHeight = gtk_widget_get_allocated_height(widget);
    scaleFactor = ((double)widgetWidth)/((double)sim->lenX);
    if (((double)widgetHeight)/((double)sim->lenY) < scaleFactor)
        scaleFactor = ((double)widgetHeight)/((double)sim->lenY);
    newWidth = (int)(scaleFactor*sim->lenX);
    newHeight = (int)(scaleFactor*sim->lenY);

    // resize into a new pixbuf
    rpixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, newWidth, newHeight);
    gdk_pixbuf_scale(pixbuf, rpixbuf,
                        0, 0,                       // location
                        newWidth, newHeight,        // size
                        0, 0,                       // offset
                        scaleFactor, scaleFactor,   // scale
                        GDK_INTERP_NEAREST);        // interpolation method

    // set the cairo source layer to our calculated pixbuf
    gdk_cairo_set_source_pixbuf(cr, rpixbuf, 0, 0);
    // fully paint the source
    cairo_paint(cr);
    // free memory
    cairo_destroy(cr);

    // invalidata everything to force redraw
    gtk_widget_queue_draw(widget);
}
