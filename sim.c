#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gtk/gtk.h>
#include "main.h"

/*==============================================================================
Reactions:
    - reactions are stored in both a weighted adjacency matrix where each cell
        corresponds to the ordered pair (source, sink) and a reaction list
        (similar to an adjacency list). The latter is needed just to have a list
        and number of reactions, to choose a reaction randomly from.
    -
==============================================================================*/

void sim_load_point( sim_t* sim, int x, int y ) {
    // load information on the local environment

    sim->local[0] = sim->env[x][y];
	if (y==0)
		sim->local[1] = sim->env[x][sim->lenY-1];	// edge wraps around
	else
		sim->local[1] = sim->env[x][y-1];
	if (x==(sim->lenX-1))
		sim->local[2] = sim->env[0][y];				// edge wraps around
	else
		sim->local[2] = sim->env[x+1][y];
	if (y==(sim->lenY-1))
		sim->local[3] = sim->env[x][0];				// edge wraps around
	else
		sim->local[3] = sim->env[x][y+1];
	if (x==0)
		sim->local[4] = sim->env[sim->lenX-1][y];	// edge wraps around
	else
		sim->local[4] = sim->env[x-1][y];

}

void sim_run( sim_t* sim ) {
    int i;
    int x, y, neighbourState;
    int reaction;
    int rateConfirm;
    float rate;

    for (i=0; i<(sim->lenX*sim->lenY); i++) {
        // randomly select a reaction to run
            // A single timestep can be set up to include nxn attemps for each
            // reaction or nxn attemps for all reactions. Here, I use the latter
            // just so we have more time resolution. Either one works, just changes
            // the time axis scale
        reaction = (rand()%(sim->nReactions));
        int source = sim->reactionList[reaction].source;
        int sink = sim->reactionList[reaction].sink;
        for (rate = sim->weightMatrix[source][sink]; rate > 0.0; rate -= 1.0) {
            x = (rand() % sim->lenX);
            y = (rand() % sim->lenY);
            sim_load_point( sim, x, y );
            neighbourState = sim->local[ (rand()%(sim->nLocal)) + 1];

            // rng is slow so only calculate if the rate is less than 1
            rateConfirm = 1;
            if (rate < 1.0)
                if (rand()/(float)RAND_MAX > rate)
                    rateConfirm = 0;

            // NOTE: this check is very hard-coded, could use some modularity
                // to handle cases more generically; do this later
            // special case for death rate since it doesn't depend on neighbours
            if (sink == 0)
                neighbourState = 0;
            if (sim->local[0] == source && neighbourState == sink
                                        && rateConfirm == 1) {
                sim->env[x][y] = sink;
            }
        }
    }
}

void sim_generate_reaction_list( sim_t* sim ) {
    int i, j;
    sim->nReactions = 0;
    for( i=0; i<sim->nSpecies + 1; i++ ) {
        for( j=0; j<sim->nSpecies + 1; j++ ) {
            if( sim->weightMatrix[i][j] != 0.0) {
                sim->reactionList[sim->nReactions].source = i;
                sim->reactionList[sim->nReactions].sink = j;
                sim->nReactions++;
            }
        }
    }
}

gboolean sim_loop( gpointer data ) {
    sim_t* sim = (sim_t*) data;

    if(sim->active) {
        sim_run(sim);
        draw_env (sim->ui.drawingArea, sim);
    }


    return TRUE; // never remove this from the idle loop
}

void sim_init( sim_t* sim ) {
    // setup simulation environment
    sim->active = FALSE;
    sim->nSpecies = 3;
    sim->lenX = 256;
    sim->lenY = 256;
    sim->specieColor = allocateArrayRGBA( sim->nSpecies );
    //sim->specieColor->red = allocateArray1DInt(sim->nSpecies + 1);
    //sim->specieColor->green = allocateArray1DInt(sim->nSpecies + 1);
    //sim->specieColor->blue = allocateArray1DInt(sim->nSpecies + 1);
    //memcpy(sim->specieColor->red, (int []){0, 0, 200, 0}, (sim->nSpecies + 1)*sizeof(int));
    //memcpy(sim->specieColor->green, (int []){0, 200, 0, 0}, (sim->nSpecies + 1)*sizeof(int));
    //memcpy(sim->specieColor->blue, (int []){0, 0, 0, 200}, (sim->nSpecies + 1)*sizeof(int));
    sim->weightMatrix = allocateArray2D(sim->nSpecies + 1, sim->nSpecies + 1);
    memcpy(sim->weightMatrix[0], (float []){0.0, 4.0, 0.0, 0.0}, 4*sizeof(float));
    memcpy(sim->weightMatrix[1], (float []){0.01, 0.0, 10.0, 0.0}, 4*sizeof(float));
    memcpy(sim->weightMatrix[2], (float []){1.0, 0.0, 0.0, 39.0}, 4*sizeof(float));
    memcpy(sim->weightMatrix[3], (float []){1.0, 0.0, 0.0, 0.0}, 4*sizeof(float));
    sim->reactionList = allocateTupleList((sim->nSpecies + 1)*(sim->nSpecies + 1));

    // create reaction list from interaction matrix
        // this also fills in the number of reactions that form
    sim_generate_reaction_list(sim);

    // allocate simulation environment
    sim->env = allocateArray2DInt(sim->lenX, sim->lenY);
    sim->nLocal = 4;
    sim->local = allocateArray1DInt(sim->nLocal + 1);
    randomizeArray2DInt(sim->env, sim->lenX, sim->lenY, 10, 10, 10);
}
