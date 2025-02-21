use <../../ktane-3d-models/bomb_base.scad>;
include <../../ktane-3d-models/variables.scad>

$fn = $preview ? 50 : 100;

tolerance = 0.3;

button_radius = 3;
button_count = 4;
button_distance = 15.6;
button_position_x = 12.6;
button_position_y = 14;

large_display_height = 25;
large_display_width = 18.8;
large_display_position_x = 36;
large_display_position_y = 65.6;

small_display_height = 19.1;
small_display_width = 12.6;
small_display_distance = small_display_width + 3;
small_display_position_x = 12.6;
small_display_position_y = 31.5;

stage_led_height = 6;
stage_led_width = 10;
stage_led_distance = 10;
stage_led_count = 5;
stage_led_position_x = 79;
stage_led_position_y = 15;

stage_led_cover_height_above_pcb = 8.2;
stage_led_cover_height_above_module = 1.8;
stage_led_cover_skirt = 2;
stage_led_cover_hole_length = 7;
stage_led_cover_tolerance = 0.1;
stage_led_cover_top_hole_radius = 1.5;
stage_led_cover_top_hole_height = 0.5;
stage_led_cover_corner_radius = 0.5;

display_cover_height = 2;
display_cover_length = 45;
display_cover_width = 34;
display_cover_corner_radius = 1;

module top() {
    difference() {
        bomb_module_top(height_above_pcb = 8, module_type = 1);
        for (i = [0:stage_led_count - 1]) {
            translate([stage_led_position_x, stage_led_position_y + stage_led_distance * i, 0]) {
                cube([stage_led_width + tolerance, stage_led_height + tolerance, 10], center = true);
            }
        }
        for (i = [0:button_count - 1]) {
            translate([button_position_x + button_distance * i, button_position_y, 0]) {
                cylinder(10, button_radius + tolerance / 2, button_radius + tolerance / 2, center = true);
            }
            translate([small_display_position_x + small_display_distance * i, small_display_position_y, 0]) {
                cube([small_display_width + tolerance, small_display_height + tolerance, 10], center = true);
            }
        }
        translate([large_display_position_x, large_display_position_y, 0]) {
            cube([large_display_width + tolerance, large_display_height + tolerance, 10], center = true);
        }
    }
}

module bottom() {
    bomb_module_bottom(height_above_pcb = 8);
}

module stage_led_cover() {
    difference() {
        union() {
            minkowski() {
                translate([0, 0, (stage_led_cover_height_above_module + stage_led_cover_height_above_pcb - stage_led_cover_corner_radius) / 2]) cube([stage_led_width - 2 * stage_led_cover_corner_radius, stage_led_height - 2 * stage_led_cover_corner_radius, stage_led_cover_height_above_pcb + stage_led_cover_height_above_module - stage_led_cover_corner_radius], center = true);
                difference() {
                    sphere(stage_led_cover_corner_radius);
                    translate([0, 0, -stage_led_cover_corner_radius]) cube([2 * stage_led_cover_corner_radius, 2 * stage_led_cover_corner_radius, 2 * stage_led_cover_corner_radius], center = true);
                }
            }
            translate([0, 0, (stage_led_cover_height_above_pcb - wall_thickness - stage_led_cover_tolerance) / 2]) cube([stage_led_width + 2 * stage_led_cover_skirt, stage_led_height / 2, stage_led_cover_height_above_pcb - wall_thickness - stage_led_cover_tolerance], center = true);
        }
        translate([0, 0, stage_led_cover_height_above_pcb / 2]) cube([stage_led_cover_hole_length, 2 * stage_led_height, stage_led_cover_height_above_pcb], center = true);
        translate([0, 0, stage_led_cover_height_above_pcb]) cylinder(stage_led_cover_top_hole_height, stage_led_cover_top_hole_radius, stage_led_cover_top_hole_radius);
    }
}

module display_cover() {
    difference() {
        minkowski() {
            translate([0, 0, (display_cover_height - display_cover_corner_radius) / 2]) cube([display_cover_length - 2 * display_cover_corner_radius, display_cover_width - 2 * display_cover_corner_radius, display_cover_height - display_cover_corner_radius], center = true);
            sphere(display_cover_corner_radius);
        }
        cube([large_display_width + tolerance, large_display_height + tolerance, 4 * display_cover_height], center = true);
        translate([0, 0, -5]) cube([2 * display_cover_length, 2 * display_cover_width, 10], center = true);
    }
}

color("darkgray") top();
color("darkgray") bottom();
cylinder_button(1.8, 4, 3, 2, 0.5);
stage_led_cover();
display_cover();