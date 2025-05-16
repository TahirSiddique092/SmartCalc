#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ENTRIES 100
#define MAX_RECORDS 100

typedef struct {
    char name[50];
    double value;
    char type;
} Entry;

typedef struct {
    int id;
    char title[100];
    int entryCount;
    char date[20];
    double total;
    double income;
    double expense;
    Entry entries[MAX_ENTRIES];
} Record;

Record records[MAX_RECORDS];          
Record search_results[MAX_RECORDS];    
int record_count = 0;                  
int search_count = 0;  

Entry entries[MAX_ENTRIES];
int entry_count = 0;
double total = 0;
double income = 0;
double expense = 0;

//GUI Functions
GtkWidget* initializeApp(GtkApplication *app, GtkWidget **flowbox_out);
void on_activate(GtkApplication *app, gpointer user_data);
void update_flowbox_cards(GtkWidget *flowbox, Record recordArr[], int count);
void search_records(GtkButton *button, gpointer user_data);
void on_submit_clicked(GtkButton *button, gpointer user_data);
void ask_title(GtkWidget *fab_button, gpointer user_data);
void on_delete_clicked(GtkButton *button, gpointer user_data);
void on_card_clicked(GtkButton *card_button, gpointer user_data);
GtkWidget* create_record_widget(char name[], double value, char type, int index, GtkWidget *flowbox);
GtkWidget* create_playgroud(GtkWidget *main_container, GtkWidget *original_scroll_widget, GtkWidget *fab_button, GtkWidget *search_container, int id);
void on_toggle_button_toggled(GtkToggleButton *button, gpointer user_data);
void on_back_button_clicked(GtkButton *button, gpointer user_data);
void on_save_button_clicked(GtkButton *button, gpointer user_data);
void update_entries(GtkWidget *flowbox);
void update_smallbox(GtkWidget *smallbox);
void on_add_entry(GtkButton *button, gpointer user_data);
void on_delete_entry_clicked(GtkButton *button, gpointer user_data);
void on_info_dialog_ok_clicked(GtkButton *button, gpointer user_data);
void show_custom_info_dialog(GtkWindow *parent, const char *message);
void apply_css(void);

//Processing Functions
int load_data();
void parseKeyValue(char *input, Entry *en);
void search(const char *query);
void add_record(const char *title);
void delete_record(int index);
void delete_entry(int index);
void calculate(int index);
void write_csv();
  
int load_data() {
    FILE *file = fopen("data.csv", "r");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    char line[512];
    record_count = 0;
    fgets(line, sizeof(line), file); 

    while (fgets(line, sizeof(line), file) && record_count < MAX_ENTRIES) {
        Record *re = &records[record_count];
        re->id = record_count;
        re->entryCount = 0;

        char *token = strtok(line, ",");
        if (!token) continue;
        strncpy(re->title, token, sizeof(re->title));

        token = strtok(NULL, ",");
        strncpy(re->date, token ? token : "", sizeof(re->date));

        token = strtok(NULL, ",");
        re->total = token ? atof(token) : 0.0;

        token = strtok(NULL, ",");
        re->income = token ? atof(token) : 0.0;

        token = strtok(NULL, ",");
        re->expense = token ? atof(token) : 0.0;

        while ((token = strtok(NULL, ",")) && re->entryCount < MAX_ENTRIES) {
            if (token[0] == '[' && token[strlen(token) - 1] == ']') {
                parseKeyValue(token, &re->entries[re->entryCount]);
                re->entryCount++;
            }
        }

        record_count++;
    }

    fclose(file);
    return 0;
}

void parseKeyValue(char *input, Entry *en) {
    sscanf(input, "[%49[^:]:%lf:%c]", en->name, &en->value, &en->type);
}

void search(const char *query){
    search_count = 0;
    for (int i = 0; i < record_count; i++) {
        if (strcmp(records[i].title, query)==0) {
            search_results[search_count++] = records[i];  
        }
    }
}

void add_record(const char *title){
    FILE *file = fopen("data.csv", "a");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    char date[30];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(date, sizeof(date), "%d %b %Y", t); 

    char clean_title[256];
    int j = 0;
    for (int i = 0; title[i] != '\0' && j < sizeof(clean_title) - 1; i++) {
        if (title[i] != ',') {
            clean_title[j++] = title[i];
        }
    }
    clean_title[j] = '\0';

    fprintf(file, "%s,%s,0,0,0,\n", clean_title, date);

    fclose(file);
}

void delete_record(int index){
    printf("Will delete %d record in future!\n",index);
}

void delete_entry(int index) {
    if (index < 0 || index >= entry_count) {
        printf("Invalid index\n");
        return;
    }

    for (int i = index; i < entry_count - 1; i++) {
        entries[i] = entries[i + 1];  
    }

    entry_count--;  
}

void calculate(int index) {
    printf("Will Calculate %d record in the future\n", index);
}

void write_csv(){
    printf("Will re-write the entire csv in future\n");
}

int main(int argc, char **argv) {
    if (load_data() == 0) {
        printf("Successfully loaded %d records.\n", record_count);
    } else {
        printf("Failed to load Data.\n");
    }

    GtkApplication *app = gtk_application_new("com.example.GtkNavbar", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);  
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}

void on_activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *flowbox;
    GtkWidget *window = initializeApp(app, &flowbox);
    apply_css();

    update_flowbox_cards(flowbox, records, record_count); 
    gtk_window_present(GTK_WINDOW(window));
}

GtkWidget* initializeApp(GtkApplication *app, GtkWidget **flowbox_out) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Smart Calc");
    gtk_window_fullscreen(GTK_WINDOW(window));
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_widget_add_css_class(window, "window");

    GtkWidget *overlay = gtk_overlay_new();
    gtk_window_set_child(GTK_WINDOW(window), overlay);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_overlay_set_child(GTK_OVERLAY(overlay), vbox);
    g_object_set_data(G_OBJECT(window), "main_container", vbox);

    GtkWidget *navbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_add_css_class(navbar, "navbar");
    gtk_box_append(GTK_BOX(vbox), navbar);

    GtkWidget *label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<span foreground='white'>SMART</span> <span foreground='#b8860a'>CALC</span>");
    gtk_widget_add_css_class(label, "navbar-label");
    gtk_box_append(GTK_BOX(navbar), label);

    GtkWidget *search_box = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(search_box), "Search...");
    gtk_widget_add_css_class(search_box, "search-box"); 

    GtkWidget *search_button = gtk_button_new_with_label("🔍");
    gtk_widget_set_size_request(search_button, 40, 30);
    gtk_widget_add_css_class(search_button, "search-button"); 

    GtkWidget *search_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_append(GTK_BOX(search_container), search_box);
    gtk_box_append(GTK_BOX(search_container), search_button);
    gtk_box_append(GTK_BOX(navbar), search_container);
    gtk_widget_add_css_class(search_container, "search-container"); 
    gtk_widget_set_halign(search_container, GTK_ALIGN_END);
    gtk_widget_set_valign(search_container, GTK_ALIGN_CENTER); 

    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_box_append(GTK_BOX(vbox), scroll);

    GtkWidget *flowbox = gtk_flow_box_new();
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(flowbox), 5);
    gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(flowbox), 1);
    gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(flowbox), GTK_SELECTION_NONE);
    gtk_flow_box_set_column_spacing(GTK_FLOW_BOX(flowbox), 20);  
    gtk_flow_box_set_row_spacing(GTK_FLOW_BOX(flowbox), 70);   
    gtk_widget_set_vexpand(flowbox, FALSE);   
    gtk_widget_set_margin_top(flowbox, 20);
    gtk_widget_set_margin_bottom(flowbox, 20);
    gtk_widget_set_margin_start(flowbox, 40);
    gtk_widget_set_margin_end(flowbox, 20);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), flowbox);
    int desired_vertical_gap = 40;
    gtk_flow_box_set_row_spacing(GTK_FLOW_BOX(flowbox), desired_vertical_gap);
    gtk_widget_set_valign(flowbox, GTK_ALIGN_START); 

    GtkWidget *fab_button = gtk_button_new_with_label("+");
    gtk_widget_set_name(fab_button, "fab"); 
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), fab_button);
    gtk_widget_set_halign(fab_button, GTK_ALIGN_END);
    gtk_widget_set_valign(fab_button, GTK_ALIGN_END);
    gtk_widget_set_margin_bottom(fab_button, 30);
    gtk_widget_set_margin_end(fab_button, 30);
    gtk_widget_add_css_class(fab_button, "fab"); 

    g_signal_connect(search_button, "clicked", G_CALLBACK(search_records), search_box);
    g_signal_connect(fab_button, "clicked", G_CALLBACK(ask_title), window);
    g_object_set_data(G_OBJECT(fab_button), "flowbox", flowbox);

    g_object_set_data(G_OBJECT(search_button), "search_entry", search_box);
    g_object_set_data(G_OBJECT(search_button), "flowbox", flowbox);
    g_object_set_data(G_OBJECT(search_button), "records", records);
    g_object_set_data(G_OBJECT(search_button), "record_count", GINT_TO_POINTER(record_count));

    g_object_set_data(G_OBJECT(flowbox), "search_container", search_container);
    g_object_set_data(G_OBJECT(flowbox), "fab_button", fab_button);
    
    *flowbox_out = flowbox;
    return window;
}

void update_flowbox_cards(GtkWidget *flowbox, Record recordArr[], int count) {
    GtkWidget *child = gtk_widget_get_first_child(flowbox);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_widget_unrealize(child);
        gtk_flow_box_remove(GTK_FLOW_BOX(flowbox), child);
        child = next;
    }

    for (int i = 0; i < count; i++) {
        GtkWidget *card_container = gtk_overlay_new();
        gtk_widget_add_css_class(card_container, "card-container");

        GtkWidget *card_button = gtk_button_new();
        gtk_widget_add_css_class(card_button, "card");
        gtk_widget_set_size_request(card_button, 150, 190);
        gtk_widget_set_vexpand(card_button, FALSE);    
        gtk_widget_set_hexpand(card_button, FALSE);
        gtk_widget_set_valign(card_button, GTK_ALIGN_START); 
        gtk_widget_set_halign(card_button, GTK_ALIGN_START); 
        g_object_set_data(G_OBJECT(card_button), "record_id", GINT_TO_POINTER(recordArr[i].id));
        g_object_set_data(G_OBJECT(card_button), "flowbox", flowbox);

        g_signal_connect(card_button, "clicked", G_CALLBACK(on_card_clicked), flowbox);  

        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        gtk_widget_add_css_class(box, "card");

        char content[256];
        snprintf(content, sizeof(content),
            "<b>%s</b>\n\nEntries: %d\nCreated On:\n<i>%s</i>\n\n<span foreground='#b8860a'>Total = %.0f</span>",
            recordArr[i].title, recordArr[i].entryCount, recordArr[i].date, recordArr[i].total);

        GtkWidget *label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(label), content);
        gtk_label_set_xalign(GTK_LABEL(label), 0.5);
        gtk_label_set_yalign(GTK_LABEL(label), 0.5);
        gtk_box_append(GTK_BOX(box), label);

        gtk_button_set_child(GTK_BUTTON(card_button), box);

        gtk_overlay_set_child(GTK_OVERLAY(card_container), card_button);

        GtkWidget *delete_btn = gtk_button_new_with_label("×");
        gtk_widget_set_size_request(delete_btn, 10, 10);
        gtk_widget_set_vexpand(delete_btn, FALSE);    
        gtk_widget_set_hexpand(delete_btn, FALSE);
        gtk_widget_set_valign(delete_btn, GTK_ALIGN_START); 
        gtk_widget_set_halign(delete_btn, GTK_ALIGN_START); 
        gtk_widget_add_css_class(delete_btn, "delete-button");
        g_object_set_data(G_OBJECT(delete_btn), "record_id", GINT_TO_POINTER(recordArr[i].id));
        g_object_set_data(G_OBJECT(delete_btn), "flowbox", flowbox);
        g_signal_connect(delete_btn, "clicked", G_CALLBACK(on_delete_clicked), NULL);
        gtk_overlay_add_overlay(GTK_OVERLAY(card_container), delete_btn);

        gtk_flow_box_append(GTK_FLOW_BOX(flowbox), card_container);

    }
}

void on_submit_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *entry = GTK_WIDGET(g_object_get_data(G_OBJECT(button), "entry"));
    GtkWidget *dialog = GTK_WIDGET(g_object_get_data(G_OBJECT(button), "dialog"));
    GtkWidget *flowbox = GTK_WIDGET(g_object_get_data(G_OBJECT(button), "flowbox"));

    const char *title = gtk_editable_get_text(GTK_EDITABLE(entry));
    if (title && strlen(title) > 0) {
        add_record(title);
        load_data();
        update_flowbox_cards(flowbox, records, record_count);
    }

    gtk_window_destroy(GTK_WINDOW(dialog));
}

void ask_title(GtkWidget *fab_button, gpointer user_data) {
    GtkWidget *dialog = gtk_window_new();
    GtkWidget *parent_window = GTK_WIDGET(user_data);
    gtk_window_set_title(GTK_WINDOW(dialog), "Add Record");
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent_window));
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
    gtk_window_set_default_size(GTK_WINDOW(dialog), 300, 150);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(main_box, 20);
    gtk_widget_set_margin_bottom(main_box, 20);
    gtk_widget_set_margin_start(main_box, 20);
    gtk_widget_set_margin_end(main_box, 20);
    gtk_window_set_child(GTK_WINDOW(dialog), main_box);

    GtkWidget *label = gtk_label_new("Enter title:");
    gtk_box_append(GTK_BOX(main_box), label);
    gtk_widget_add_css_class(label, "popup-label");

    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Title");
    gtk_box_append(GTK_BOX(main_box), entry);

    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(button_box, GTK_ALIGN_END);
    gtk_box_append(GTK_BOX(main_box), button_box);

    GtkWidget *submit_button = gtk_button_new_with_label("Submit");
    gtk_box_append(GTK_BOX(button_box), submit_button);

    GtkWidget *flowbox = GTK_WIDGET(g_object_get_data(G_OBJECT(fab_button), "flowbox"));

    g_object_set_data(G_OBJECT(submit_button), "entry", entry);
    g_object_set_data(G_OBJECT(submit_button), "dialog", dialog);
    g_object_set_data(G_OBJECT(submit_button), "flowbox", flowbox);

    g_signal_connect(submit_button, "clicked", G_CALLBACK(on_submit_clicked), dialog);

    gtk_window_present(GTK_WINDOW(dialog));
}

void search_records(GtkButton *button, gpointer user_data) {
    GtkEntry *search_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(button), "search_entry"));
    GtkWidget *flowbox = GTK_WIDGET(g_object_get_data(G_OBJECT(button), "flowbox"));
    GtkWidget *parent_window = GTK_WIDGET(g_object_get_data(G_OBJECT(button), "parent_window"));

    const char *query = gtk_editable_get_text(GTK_EDITABLE(search_entry));

    if (strlen(query) == 0) {
        update_flowbox_cards(flowbox, records, record_count);
    }else{
        search(query);

        if (search_count == 0) {
            show_custom_info_dialog(GTK_WINDOW(parent_window), "No matching records found.");
        } else {
            update_flowbox_cards(flowbox, search_results, search_count);
        }
    }
}

void on_delete_clicked(GtkButton *button, gpointer user_data) {
    int id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "record_id"));

    delete_record(id);
    load_data();

    update_flowbox_cards(GTK_WIDGET(g_object_get_data(G_OBJECT(button), "flowbox")), records, record_count);
}

void on_delete_entry_clicked(GtkButton *button, gpointer user_data) {
    int id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "entry_id"));

    delete_entry(id);

    update_entries(GTK_WIDGET(g_object_get_data(G_OBJECT(button), "flowbox")));
}

void on_card_clicked(GtkButton *card_button, gpointer user_data) {
    int id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(card_button), "record_id"));

    GtkWidget *flowbox = GTK_WIDGET(user_data);
    GtkWidget *window = gtk_widget_get_ancestor(flowbox, GTK_TYPE_WINDOW);
    GtkWidget *main_container = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "main_container"));

    GtkWidget *viewport = gtk_widget_get_parent(flowbox); 
    GtkWidget *actual_scroll_widget = gtk_widget_get_parent(viewport);
    
    GtkWidget *fab_button = GTK_WIDGET(g_object_get_data(G_OBJECT(flowbox), "fab_button"));
    GtkWidget *search_container = GTK_WIDGET(g_object_get_data(G_OBJECT(flowbox), "search_container"));

    if (actual_scroll_widget && GTK_IS_WIDGET(actual_scroll_widget)) {
        gtk_widget_set_visible(actual_scroll_widget, FALSE);
    }

    if (fab_button) {
        gtk_widget_set_visible(fab_button, FALSE);
    }

    if(search_container){
        gtk_widget_set_visible(search_container, FALSE);
    }

    GtkWidget *new_playground_layout = create_playgroud(main_container, actual_scroll_widget, fab_button, search_container, id);
    gtk_widget_set_vexpand(new_playground_layout, TRUE);
    gtk_box_append(GTK_BOX(main_container), new_playground_layout);
}

GtkWidget* create_playgroud(GtkWidget *main_container, GtkWidget *original_scroll_widget, GtkWidget *fab_button, GtkWidget *search_container, int id) {
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    GtkWidget *flowbox = gtk_flow_box_new();
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(flowbox), 1);
    gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(flowbox), GTK_SELECTION_NONE);
    int desired_vertical_gap = 12;
    gtk_flow_box_set_row_spacing(GTK_FLOW_BOX(flowbox), desired_vertical_gap);
    gtk_widget_set_valign(flowbox, GTK_ALIGN_START); 

    Record re = records[id];

    memcpy(entries, re.entries, re.entryCount * sizeof(Entry));
    entry_count = re.entryCount;

    total = re.total;
    income = re.income;
    expense = re.expense;

    update_entries(flowbox);

    GtkWidget *small_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_widget_add_css_class(small_box, "small_card");
    gtk_widget_set_size_request(small_box, 150, 200);
    gtk_widget_set_vexpand(small_box, FALSE);    
    gtk_widget_set_hexpand(small_box, FALSE);

    update_smallbox(small_box);

    gtk_widget_set_halign(small_box, GTK_ALIGN_START);
    gtk_widget_set_valign(small_box, GTK_ALIGN_END);
    gtk_widget_set_margin_start(small_box, 30);
    gtk_widget_set_margin_bottom(small_box, 70);


    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), flowbox);
    gtk_widget_set_size_request(scrolled_window, 600, 600);
    gtk_widget_set_name(scrolled_window, "playground-scroll");

    GtkWidget *center_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(scrolled_window, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(scrolled_window, GTK_ALIGN_FILL);
    gtk_box_append(GTK_BOX(center_box), scrolled_window);

    gtk_box_append(GTK_BOX(main_box), center_box);
    gtk_widget_set_halign(main_box, GTK_ALIGN_CENTER);  
    gtk_widget_set_valign(main_box, GTK_ALIGN_CENTER);

    GtkWidget *overlay = gtk_overlay_new();
    gtk_overlay_set_child(GTK_OVERLAY(overlay), main_box);

    GtkWidget *back_button = gtk_button_new_from_icon_name("go-previous-symbolic");
    gtk_widget_add_css_class(back_button, "back-button"); 
    gtk_widget_set_halign(back_button, GTK_ALIGN_START);
    gtk_widget_set_valign(back_button, GTK_ALIGN_START);
    gtk_widget_set_margin_start(back_button, 20);
    gtk_widget_set_margin_top(back_button, 20);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), back_button);

    g_object_set_data(G_OBJECT(back_button), "main_container", main_container);
    g_object_set_data(G_OBJECT(back_button), "overlay", overlay);
    g_object_set_data(G_OBJECT(back_button), "original_scroll_widget", original_scroll_widget);
    g_object_set_data(G_OBJECT(back_button), "fab_button", fab_button);
    g_object_set_data(G_OBJECT(back_button), "search_container", search_container);

    g_signal_connect(back_button, "clicked", G_CALLBACK(on_back_button_clicked), back_button); 

    GtkWidget *save_button = gtk_button_new_with_label("✓");
    gtk_widget_add_css_class(save_button, "save-button");
    gtk_widget_set_halign(save_button, GTK_ALIGN_END);
    gtk_widget_set_valign(save_button, GTK_ALIGN_END);
    gtk_widget_set_margin_end(save_button, 30);
    gtk_widget_set_margin_bottom(save_button, 100);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), save_button);

    g_object_set_data(G_OBJECT(save_button), "index", GINT_TO_POINTER(id));
    g_object_set_data(G_OBJECT(save_button), "flowbox", flowbox);
    g_object_set_data(G_OBJECT(save_button), "small_box", small_box);

    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_button_clicked), save_button); 

    GtkWidget *add_button = gtk_button_new_with_label("+");
    gtk_widget_add_css_class(add_button, "add-button");
    gtk_widget_set_halign(add_button, GTK_ALIGN_END);
    gtk_widget_set_valign(add_button, GTK_ALIGN_END);
    gtk_widget_set_margin_end(add_button, 30);
    gtk_widget_set_margin_bottom(add_button, 30);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), add_button);
    g_signal_connect(add_button, "clicked", G_CALLBACK(on_add_entry), flowbox);

    GtkWidget *title_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_widget_add_css_class(title_box, "title_box");
    gtk_widget_set_size_request(title_box, 250, 50);
    gtk_widget_set_vexpand(title_box, FALSE);    
    gtk_widget_set_hexpand(title_box, FALSE);
    gtk_widget_set_halign(title_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(title_box, GTK_ALIGN_START);
    gtk_widget_set_margin_start(title_box, 30);
    gtk_widget_set_margin_top(title_box, 0);

    char titleContent[256];
    snprintf(titleContent, sizeof(titleContent),"<b>%s</b>",records[id].title);

    GtkWidget *title_label = gtk_label_new(NULL);
    gtk_widget_add_css_class(title_label, "title_label");
    gtk_label_set_markup(GTK_LABEL(title_label), titleContent);
    gtk_label_set_xalign(GTK_LABEL(title_label), 0.5);
    gtk_label_set_yalign(GTK_LABEL(title_label), 0.5);
    gtk_box_append(GTK_BOX(title_box), title_label);

    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), title_box);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), small_box);

    return overlay;
}

void update_entries(GtkWidget *flowbox) {
    GtkWidget *child = gtk_widget_get_first_child(flowbox);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_widget_unrealize(child);
        gtk_flow_box_remove(GTK_FLOW_BOX(flowbox), child);
        child = next;
    }

    for (int i = 0; i < entry_count ; i++) {
        Entry en = entries[i];
        GtkWidget *new_card = create_record_widget(en.name, en.value, en.type, i, flowbox);
        gtk_flow_box_insert(GTK_FLOW_BOX(flowbox), new_card, -1);
    }
}

void update_smallbox(GtkWidget *smallbox) {
    GtkWidget *child = gtk_widget_get_first_child(smallbox);
    if(child != NULL) {
        gtk_box_remove(GTK_BOX(smallbox), child);
    }

    char content[256];
    snprintf(content, sizeof(content),
        "\n🟢<b>  Income = %.0f</b>\n\n🟡<b>  Expense =  %.0f\n\n    <span foreground='#b8860a'>Total = %.0f</span></b>",
        income, expense, total);

    GtkWidget *small_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(small_label), content);
    gtk_label_set_xalign(GTK_LABEL(small_label), 0.5);
    gtk_label_set_yalign(GTK_LABEL(small_label), 0.5);
    gtk_box_append(GTK_BOX(smallbox), small_label);
}


GtkWidget* create_record_widget(char name[], double value, char type, int index, GtkWidget *flowbox) {

    GtkWidget *overlay = gtk_overlay_new();
    gtk_widget_set_focusable(overlay, FALSE);


    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER); 
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

    GtkWidget *entry1 = gtk_entry_new();
    GtkEntryBuffer *buffer1 = gtk_entry_get_buffer(GTK_ENTRY(entry1));
    gtk_entry_buffer_set_text(buffer1, name, -1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry1), "Title");

    GtkWidget *entry2 = gtk_entry_new();
    char value_str[64];
    snprintf(value_str, sizeof(value_str), "%.2f", value);  
    GtkEntryBuffer *buffer2 = gtk_entry_get_buffer(GTK_ENTRY(entry2));
    gtk_entry_buffer_set_text(buffer2, value_str, -1); 
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry2), "Value");
    

    GtkWidget *toggle_button = gtk_toggle_button_new_with_label(type == 'I' ? "I" : "E"); 
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle_button), type == 'I');
    
    if (type == 'I') {
        gtk_widget_add_css_class(toggle_button, "toggle-button-green");  
    } else {
        gtk_widget_add_css_class(toggle_button, "toggle-button-yellow");  
    }
    

    gtk_box_append(GTK_BOX(box), entry1);
    gtk_box_append(GTK_BOX(box), entry2);
    gtk_box_append(GTK_BOX(box), toggle_button);
    g_signal_connect(toggle_button, "toggled", G_CALLBACK(on_toggle_button_toggled), NULL);

    gtk_overlay_set_child(GTK_OVERLAY(overlay), box);

    GtkWidget *delete_entry = gtk_button_new_with_label("×");
    gtk_widget_set_size_request(delete_entry, 20, 20); 
    gtk_widget_set_valign(delete_entry, GTK_ALIGN_START); 
    gtk_widget_set_halign(delete_entry, GTK_ALIGN_START); 
    gtk_widget_add_css_class(delete_entry, "delete-button");
    gtk_widget_set_margin_top(delete_entry, 0);
    gtk_widget_set_margin_start(delete_entry, 30);
    
    g_object_set_data(G_OBJECT(delete_entry), "entry_id", GINT_TO_POINTER(index));
    g_object_set_data(G_OBJECT(delete_entry), "flowbox", flowbox);
    g_signal_connect(delete_entry, "clicked", G_CALLBACK(on_delete_entry_clicked), NULL);
    
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), delete_entry);

    gtk_widget_add_css_class(box, "record-item");
    gtk_widget_add_css_class(entry1, "record-title-entry");
    gtk_widget_add_css_class(entry2, "record-value-entry");

    gtk_widget_set_vexpand(box, FALSE);    
    gtk_widget_set_hexpand(box, FALSE);

    g_signal_connect(toggle_button, "toggled", G_CALLBACK(on_toggle_button_toggled), NULL);

    gtk_widget_set_size_request(box, 250, 50); 

    return overlay;
}


void on_back_button_clicked(GtkButton *button, gpointer user_data) {

    GtkWidget *main_container = GTK_WIDGET(g_object_get_data(G_OBJECT(button), "main_container"));
    GtkWidget *overlay = GTK_WIDGET(g_object_get_data(G_OBJECT(button), "overlay"));
    GtkWidget *original_scroll_widget = GTK_WIDGET(g_object_get_data(G_OBJECT(button), "original_scroll_widget"));
    GtkWidget *fab_button = GTK_WIDGET(g_object_get_data(G_OBJECT(button), "fab_button")); 
    GtkWidget *search_container = GTK_WIDGET(g_object_get_data(G_OBJECT(button), "search_container"));

    if ( !main_container || !overlay || !original_scroll_widget || !fab_button) {
        g_warning("Invalid data received in on_back_button_clicked.\n");
        return;
    }

    if (gtk_widget_get_parent(overlay)) {
        gtk_widget_unparent(overlay);
    } else {
        g_warning("Playground layout has no parent.\n");
    }

    gtk_widget_set_visible(original_scroll_widget, TRUE);
    

    gtk_widget_set_visible(search_container, TRUE);
    gtk_widget_set_visible(fab_button, TRUE);

}

void on_save_button_clicked(GtkButton *button, gpointer user_data) {
   

    int record_id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "index"));
    GtkWidget *flowbox = GTK_WIDGET(g_object_get_data(G_OBJECT(button), "flowbox"));
    GtkWidget *smallbox = GTK_WIDGET(g_object_get_data(G_OBJECT(button), "small_box")); 

    entry_count = 0;

    GtkWidget *flowbox_child_item = gtk_widget_get_first_child(flowbox);
    while (flowbox_child_item != NULL) {
        GtkWidget *actual_overlay_widget = NULL;
        if (GTK_IS_FLOW_BOX_CHILD(flowbox_child_item)) {
            actual_overlay_widget = gtk_flow_box_child_get_child(GTK_FLOW_BOX_CHILD(flowbox_child_item));
        } else {
            g_warning("Item in flowbox is not a GtkFlowBoxChild. Skipping.");
            flowbox_child_item = gtk_widget_get_next_sibling(flowbox_child_item);
            continue;
        }

        if (actual_overlay_widget && GTK_IS_OVERLAY(actual_overlay_widget)) {
            GtkWidget *record_item_box = gtk_overlay_get_child(GTK_OVERLAY(actual_overlay_widget));
            if (record_item_box && GTK_IS_BOX(record_item_box)) {
                GtkWidget *entry1_widget = gtk_widget_get_first_child(record_item_box);
                GtkWidget *entry2_widget = entry1_widget ? gtk_widget_get_next_sibling(entry1_widget) : NULL;
                GtkWidget *toggle_button_widget = entry2_widget ? gtk_widget_get_next_sibling(entry2_widget) : NULL;

                if (GTK_IS_EDITABLE(entry1_widget) && GTK_IS_EDITABLE(entry2_widget) && GTK_IS_TOGGLE_BUTTON(toggle_button_widget)) {
                    const char *name_str = gtk_editable_get_text(GTK_EDITABLE(entry1_widget));
                    const char *value_str = gtk_editable_get_text(GTK_EDITABLE(entry2_widget));
                    double value = atof(value_str);
                    char type = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggle_button_widget)) ? 'I' : 'E';

                    if (entry_count < MAX_ENTRIES) {
                        if (strlen(name_str) != 0) {
                            Entry *temp_entry_data = &entries[entry_count];
                            strncpy(temp_entry_data->name, name_str, sizeof(temp_entry_data->name) - 1);
                            temp_entry_data->name[sizeof(temp_entry_data->name) - 1] = '\0';
                            temp_entry_data->value = value;
                            temp_entry_data->type = type;
                            entry_count++;
                        }
                    } else {
                        g_warning("Max entries (%d) reached while reading UI. Some entries might be ignored.\n", MAX_ENTRIES);
                        break;
                    }
                } 
            } 
        } 
        flowbox_child_item = gtk_widget_get_next_sibling(flowbox_child_item);
    }

    Record *current_record = &records[record_id];

    current_record->entryCount = entry_count;

    for (int i = 0; i < entry_count; i++) {
        current_record->entries[i] = entries[i]; 
    }

    calculate(record_id);
    write_csv();
    load_data();
    update_entries(flowbox);
    update_smallbox(smallbox);

}


void on_add_entry(GtkButton *button, gpointer user_data) {
    GtkWidget *flowbox = GTK_WIDGET(user_data);
    Entry en;
    strcpy(en.name, "");
    en.value = 0.0;
    en.type = 'E';
    entries[entry_count] = en;
    entry_count++;
    update_entries(flowbox);
}

void on_toggle_button_toggled(GtkToggleButton *button, gpointer user_data) {
    gboolean active = gtk_toggle_button_get_active(button);

    if (active) {
        gtk_button_set_label(GTK_BUTTON(button), "I");

        gtk_widget_remove_css_class(GTK_WIDGET(button), "toggle-button-yellow");
        gtk_widget_add_css_class(GTK_WIDGET(button), "toggle-button-green");
    } else {
        gtk_button_set_label(GTK_BUTTON(button), "E");

        gtk_widget_remove_css_class(GTK_WIDGET(button), "toggle-button-green");
        gtk_widget_add_css_class(GTK_WIDGET(button), "toggle-button-yellow");
    }
}

void on_info_dialog_ok_clicked(GtkButton *button, gpointer user_data) {
    GtkWindow *window = GTK_WINDOW(user_data);
    if (GTK_IS_WINDOW(window)) {
        gtk_window_destroy(window);
    }
}

void show_custom_info_dialog(GtkWindow *parent, const char *message) {
    GtkWidget *window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "Info");
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(window), parent);
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 100);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(box, 20);
    gtk_widget_set_margin_bottom(box, 20);
    gtk_widget_set_margin_start(box, 20);
    gtk_widget_set_margin_end(box, 20);
    gtk_window_set_child(GTK_WINDOW(window), box);

    GtkWidget *label = gtk_label_new(message);
    gtk_box_append(GTK_BOX(box), label);
    gtk_widget_add_css_class(label, "popup-label");

    GtkWidget *button = gtk_button_new_with_label("OK");
    gtk_box_append(GTK_BOX(box), button);

    g_signal_connect(button, "clicked", G_CALLBACK(on_info_dialog_ok_clicked), window);

    gtk_window_present(GTK_WINDOW(window));
}




void apply_css(void) {
    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css_provider, "style.css");

    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );

    g_object_unref(css_provider);
}
