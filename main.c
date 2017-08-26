#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "main.h"


int main (int argc, char** argv) {
    GtkApplication* app;
    int status;

    sim_t* sim;
    sim = (sim_t*)malloc(sizeof(sim_t));
    sim->surface = NULL;
    sim->env = NULL;
    sim->ui.gridSliders = NULL;
    sim->ui.listSliders = NULL;
    sim->ui.listSpinButtons = NULL;
    sim->ui.gridPalette = NULL;
    sim->ui.listColorButton = NULL;
    // creates a GtkApplication object, with unique identifier
    app = gtk_application_new ("org.sllvm.visualizer", G_APPLICATION_FLAGS_NONE);
    // sets up the activate signal for when the app is started
    g_signal_connect (app, "activate", G_CALLBACK (app_init), sim);
    // starts up app; GTK+ parses relevant arguments and removes them, leaving
        // the unrecognized ones to be used by program
    status = g_application_run (G_APPLICATION (app), argc, argv);

    // application returns here when window is closed
    // free memory with unref
    g_object_unref (app);

    return status;
}
