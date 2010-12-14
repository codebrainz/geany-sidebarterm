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

/*
 * One "bug" I noticed but I'm not sure of why it happens is when Geany is
 * opened without the plugin enabled, then it's enabled, it's almost like it
 * presses Enter on the VTE so there are two lines.  
 * Pretty much a non-issue IMOP.
 */

#include "geanyplugin.h"

GeanyPlugin     *geany_plugin;
GeanyData       *geany_data;
GeanyFunctions  *geany_functions;

PLUGIN_VERSION_CHECK(147)

PLUGIN_SET_INFO("Sidebar Terminal", "Move the VTE terminal into the sidebar.",
                "1.0", "Matthew Brush <mbrush@leftclick.ca>");

static GtkNotebook *vte_old_home = NULL;
static GtkNotebook *vte_new_home = NULL;
static GtkWidget *vte_frame = NULL;
static GtkWidget *vte_tab_label = NULL;

/* 
 * is it better to add:
 *      ui_hookup_widget(main_widgets.window, frame, "vte_frame");
 * to the create_vte() function in vte.c (~ line 244) ?
 */
static GtkWidget *get_vte_frame(void)
{
#ifdef VTE_HOOKUP_PATCH
    /*
     * run this code if vte.c is patched to add:
     *      ui_hookup_widget(main_widgets.window, frame, "vte_frame");
     * to the create_vte() function (see vte-hookup.patch).
     */
    return ui_lookup_widget(
                    geany_data->main_widgets->window, 
                    "vte_frame");
#else
    /* in theory, the Terminal tab should be the last tab */
    GtkWidget *vfra = gtk_notebook_get_nth_page(
                GTK_NOTEBOOK(
                    geany_data->main_widgets->message_window_notebook), -1);
    /* get the label of the assumed Terminal tab */
    const gchar *labeltxt = gtk_notebook_get_tab_label_text(
                                GTK_NOTEBOOK(geany_data->main_widgets->message_window_notebook), 
                                vfra);
    /* check if we have the wrong tab (hacky?) */
    if (g_strcmp0(labeltxt, "Terminal") != 0)
        return NULL;
        
    return vfra;
#endif
}

void plugin_init(GeanyData *data)
{
        
    /* get a handle on the frame that holds the vte stuff */
    vte_frame = get_vte_frame();
    
    if (vte_frame == NULL)
    {
        g_printerr("Geany does not contain a VTE terminal, bailing out.\n");
        return;
    }
    
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
    
    /* select the new vte tab (last tab) in the sidebar */
    gtk_notebook_set_current_page(vte_new_home, -1);
}

void plugin_cleanup(void)
{
    /* move the vte frame back to where it was before */
    gtk_widget_reparent(vte_frame, GTK_WIDGET(vte_old_home)); 
    
    /* put the label back in the old notebook */
    gtk_notebook_set_tab_label(vte_old_home, vte_frame, vte_tab_label);
    
    /* we no longer to to hang on to a reference of the label */
    g_object_unref(G_OBJECT(vte_tab_label));
    
    /* select the vte tab (last tab) in the message window */
    gtk_notebook_set_current_page(vte_old_home, -1);
}
