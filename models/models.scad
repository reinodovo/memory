use <../../ktane-3d-models/bomb_base.scad>;

$fn = $preview ? 50 : 100;

tolerance = 0.4;
detail_wall_thickness = 1.5;
detail_height = 1.4;
detail_tolerance = 0.15;
detail_corner_radius = 1;
detail_skirt_size = 1;
detail_skirt_height = 1;

wall_thickness = 2;

speaker_hole_radius = 1.8 / 2;
speaker_hole_space = 3;
speaker_hole_sides = 6;

button_radius = 3.8;
button_height = 7;
button_height_above_top = 2;

module top() {
    difference() {
        bomb_module_top(height_above_pcb = 8, module_type = 1);
        // TODO: Add display and LED holes
    }
}

module bottom() {
    bomb_module_bottom(height_above_pcb = 8);
}

color("darkgray") top();
color("darkgray") bottom();
