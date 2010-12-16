/*
 * sidebarterm.c
 * 
 * Copyright 2010 Matthew Brush <mbrush@leftclick.ca>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "geanyplugin.h"
#include <vte/vte.h>

GeanyPlugin     *geany_plugin;
GeanyData       *geany_data;
GeanyFunctions *geany_functions;

#define ST_PLUGIN_NAME _("Sidebar Terminal")

PLUGIN_VERSION_CHECK(147)

PLUGIN_SET_INFO(ST_PLUGIN_NAME, _("Move the VTE terminal into the sidebar."),
                "1.0", "Matthew Brush <mbrush@leftclick.ca>");

static GtkNotebook *vte_old_home = NULL;
static GtkNotebook *vte_new_home = NULL;
static GtkWidget *vte_frame = NULL;
static GtkWidget *vte_tab_label = NULL;
static gboolean have_vte = FALSE;

static void show_error_message(void);
static GtkWidget *get_vte_frame(void);
static gboolean holds_vte(GtkWidget *frame);

void plugin_init(GeanyData *data)
{
    /* get a handle on the frame that holds the vte stuff */
    vte_frame = get_vte_frame();
    
    /* make sure it's really the frame holding the vte stuff */
    if (vte_frame == NULL || !holds_vte(vte_frame))
    {
        show_error_message();
        return;
    }
    
    /* set a flag for the cleanup function to use */
    have_vte = TRUE;
    
    /* store where the vte frame is going to go */
    vte_new_home = GTK_NOTEBOOK(geany_data->main_widgets->sidebar_notebook);
    
    /* store where the vte was so we can put it back */
    vte_old_home = GTK_NOTEBOOK(gtk_widget_get_parent(vte_frame));
    
    /* grab the notebook page label so we can set it on the new tab */
    vte_tab_label = gtk_notebook_get_tab_label(vte_old_home, vte_frame);
    
    /* increase the ref count so the label doesn't get destroy when we
     * reparent the notebook child */
    g_object_ref(G_OBJECT(vte_tab_label));
    
    /* move the vte frame to the sidebar notebook */
    gtk_widget_reparent(vte_frame, GTK_WIDGET(vte_new_home));
    
    /* set the label again since it's gone somewhere */
    gtk_notebook_set_tab_label(vte_new_home, GTK_WIDGET(vte_frame), vte_tab_label);
    
    /* select the new vte tab in the sidebar */
    gtk_notebook_set_current_page(vte_new_home, -1);
}

void plugin_cleanup(void)
{
    if (have_vte)
    {
        /* move the vte frame back to where it was before */
        gtk_widget_reparent(vte_frame, GTK_WIDGET(vte_old_home)); 
        
        /* put the label back in the old notebook */
        gtk_notebook_set_tab_label(vte_old_home, vte_frame, vte_tab_label);
        
        /* we no longer to to hang on to a reference of the label */
        g_object_unref(G_OBJECT(vte_tab_label));
        
        /* select the vte tab in the message window */
        gtk_notebook_set_current_page(vte_old_home, MSG_VTE);
    }
}

/* for when the vte cannot be located */
static void show_error_message(void)
{
    GtkWidget *dlg = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
                        GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s %s", 
                        ST_PLUGIN_NAME, _("Plugin"));
    
    gtk_message_dialog_format_secondary_markup(GTK_MESSAGE_DIALOG(dlg), "%s",
        _("There is currently no terminal loaded in Geany.  Enable the terminal "
        "in Geany's prefrences dialog and restart Geany to use the plugin "
        "or disable the plugin to stop seeing this error message."));
    
    gtk_dialog_run(GTK_DIALOG(dlg));
    gtk_widget_destroy(dlg);
}

/* find what should be the VTE notebook child (if the VTE is enabled) */
static GtkWidget *get_vte_frame(void)
{
    GtkNotebook *nb;
    nb = GTK_NOTEBOOK(geany_data->main_widgets->message_window_notebook);
    return gtk_notebook_get_nth_page(nb, MSG_VTE);
}

/* locate vte anywhere at or below widget */
static gboolean holds_vte(GtkWidget *widget)
{
    gboolean found = FALSE;
    
    if (VTE_IS_TERMINAL(widget))
        found = TRUE;
    else if (GTK_IS_CONTAINER(widget))
    {
        GList *children, *iter;
        
        children = gtk_container_get_children(GTK_CONTAINER(widget));
        
        for (iter=children; !found && iter; iter=g_list_next(iter))
            found = holds_vte(iter->data);
            
        g_list_free(children);
    }
    
    return found;
}
