# Using instruction
Type in console:
`program.exe <input_file_name> <output_file_name> <line_brightness> <line_thickness> <start_x> <start_y> <end_x> <end_y> <gamma>`, where:
<line_brightness>: integer between 0 and 255;
<line_thickness>: positive float;
(<start_x>, <start_y>), (<end_x>, <end_y>): coordinates between (0, 0) and (width, height), and (0, 0) means up-left corner, all are floats.
<gamma>(**optional**): positive float, non-existing arguement means sRGB using.
