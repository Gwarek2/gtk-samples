#include <errno.h>
#include <gtk/gtk.h>
#include <math.h>

#define WIDTH   640
#define HEIGHT  480

#define ZOOM_X  100.0
#define ZOOM_Y  100.0

#define GR_EPS 1e-7


gdouble f (gdouble x)
{
    if (fabs(x) < GR_EPS) x = 0;
    return 1 / (x);
}

static void
on_draw (GtkDrawingArea *widget,
         cairo_t        *cr,
         int            width,
         int            height,
         gpointer       data)
{
    GdkRectangle da;            /* GtkDrawingArea size */
    gdouble dx = 5.0, dy = 5.0; /* Pixels between each point */
    gdouble i, clip_x1 = 0.0, clip_y1 = 0.0, clip_x2 = 0.0, clip_y2 = 0.0;

    /* Determine GtkDrawingArea dimensions */
    gtk_window_get_default_size(GTK_WINDOW(data),
                                &da.width,
                                &da.height);

    /* Draw on a black background */
    cairo_set_source_rgb (cr, 255.0, 255.0, 255.0);
    cairo_paint (cr);

    /* Change the transformation matrix */
    cairo_translate (cr, da.width / 2, da.height / 2);
    cairo_scale (cr, ZOOM_X, -ZOOM_Y);

    /* Determine the data points to calculate (ie. those in the clipping zone) */
    cairo_device_to_user_distance (cr, &dx, &dy);
    cairo_clip_extents (cr, &clip_x1, &clip_y1, &clip_x2, &clip_y2);
    cairo_set_line_width (cr, dx / 4);

    /* Draws x and y axis */
    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
    cairo_move_to (cr, clip_x1, 0.0);
    cairo_line_to (cr, clip_x2, 0.0);
    cairo_move_to (cr, 0.0, clip_y1);
    cairo_line_to (cr, 0.0, clip_y2);
    cairo_stroke (cr);

    /* g_print("%f\n", 1.0 / 0.0); */
    /* Link each data point */
    cairo_set_line_width (cr, dx / 2);
    for (i = clip_x1; i < clip_x2; i += dx) {
        gdouble result = f(i);
        /* g_print("%f %f %f\n", i, result, f(i)); */
        if (!isnan(result) && !isinf(result))
            cairo_line_to (cr, i, result);
        else
            cairo_move_to (cr, i, result);
    }

    /* Draw the curve */
    cairo_set_source_rgba (cr, 1, 0.2, 0.2, 0.6);
    cairo_stroke (cr);
}

static void
activate (GtkApplication *app,
          gpointer       data)
{
    GtkWidget *window;
    GtkWidget *da;

    window = gtk_application_window_new (app);
    gtk_window_set_default_size(GTK_WINDOW (window), WIDTH, HEIGHT);
    gtk_window_set_title(GTK_WINDOW (window), "Graph drawing");
    g_signal_connect(G_OBJECT (window), "destroy", G_CALLBACK (gtk_window_destroy), NULL);

    da = gtk_drawing_area_new ();

    gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (da),
                                    on_draw, window,
                                    NULL);

    gtk_window_set_child (GTK_WINDOW (window), da);
    gtk_window_present (GTK_WINDOW (window));
}


int
main (int argc,
      char **argv)
{
    GtkApplication *app;

    app = gtk_application_new ("org.gtk.graph", G_APPLICATION_FLAGS_NONE);

    g_signal_connect (G_OBJECT (app), "activate", G_CALLBACK (activate), NULL);
    g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return 0;
}
