#include <gtk/gtk.h>

typedef struct {
    int source;
    int sink;
} tuple_t;


typedef struct {
    GtkWidget*  drawingArea;
    GtkWidget*  buttonInitialize;
    GtkWidget*  buttonToggle;
    GtkWidget*  expanderSliders;
    GtkWidget*  gridSliders;
    GtkWidget**  listSliders;
} ui_t;

typedef struct {
    // drawing related variables
    cairo_surface_t*    surface;

    ui_t ui;

    // flag for whether the simulation should be running
    int active;
    int nSpecies;
    int lenX;
    int lenY;
    // arrays to store species colors using rgb channels
        // should be allocated length (nSpecies+1)
        // the extra cell (0) represents empty space
    int* channelR;
    int* channelG;
    int* channelB;
    // weight matrix representing rates of reactions (birth/predation/death)
        // should be allocated of size (nSpecies+1)*(nSpecies+1)
        // the extra cell represents empty space used for birth/death rates
    float** weightMatrix;
    int nReactions;
    tuple_t* reactionList;
    // grid representing current status of species occupying each cell
        // should be allocated of size lenX*lenY
    int** env;
    // local grid to make calculations simpler (and more modular)
    int nLocal;     // number of neighbours to take,
    int* local;
} sim_t;

// main.c:

// sim.c: simulation calculations and related functions
void sim_load_point( sim_t* sim, int x, int y );
void sim_generate_reaction_list( sim_t* sim );
void sim_run( sim_t* sim );
gboolean sim_loop( gpointer data );
void sim_init( sim_t* sim );

// allocate.c: wrappers for malloc
    // mostly for generic array allocations but also have struct list allocation
// NOTE: allocateTupleList allocates nSpecies*nSpecies number of tuples because
    // the number of interactions is unknown prior to allocation and I'm too
    // lazy to make a dynamically enlarging array. There shouldn't be much
    // memory waste because even if the interaction matrix is sparse the
    // absolute number of species is likely not gonna exceed order 10
tuple_t* allocateTupleList(const int dim1);
GtkWidget** allocateWidgetList(const int dim1);
void deallocateWidgetList( GtkWidget** widget );
float* allocateArray1D(const int dim1);
int* allocateArray1DInt(const int dim1);
float** allocateArray2D(const int dim1, const int dim2);
int** allocateArray2DInt(const int dim1, const int dim2);
float*** allocateArray3D(const int dim1, const int dim2, const int dim3);
int*** allocateArray3dInt(const int dim1, const int dim2, const int dim3);

// arrays.c: generic array operations
void randomizeArray2DInt(int** array, int dim1, int dim2, int nRand1, int nRand2, int nRand3);
void printArray1D(float* array, int dim1);
void printArray1DInt(int* array, int dim1);
void printArray2DInt(int** array, int dim1, int dim2);
void copyArray2DInt(int** arrayOld, int** arrayNew, int dim1, int dim2);

// draw.c
void draw_clear(sim_t* sim);
void draw_env(GtkWidget* widget, sim_t* sim);

// window.c: gtk related functions
gboolean b_initialize_cb(GtkWidget* widget, GdkEventMotion* event, sim_t* sim);
gboolean b_run_toggle_cb(GtkWidget* widget, sim_t* sim);
gboolean s_sliders_cb(  GtkRange*       widget,
                        GtkScrollType   scroll,
                        gdouble         value,
                        sim_t*          sim);
void wr_b_detoggle (GtkButton* button, gpointer data);        // wrapper to trigger detoggling
gboolean canvas_configure_cb(GtkWidget* widget, GdkEventConfigure* event, sim_t* sim);
gboolean canvas_mouse_press_cb(GtkWidget* widget, GdkEventButton* event, sim_t* sim);
gboolean canvas_mouse_motion_cb(GtkWidget* widget, GdkEventMotion* event, sim_t* sim);
gboolean canvas_draw_cb(GtkWidget* widget, cairo_t* cr, sim_t* sim);
void close_window (sim_t* sim);
void app_init(GtkApplication* app, sim_t* sim);
void window_set_rate_sliders( sim_t* sim );
