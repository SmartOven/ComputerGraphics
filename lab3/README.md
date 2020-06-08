# Using manual
Type in console:  
`program.exe <input_file_name> <output_file_name> <gradient> <dithering> <bit> <gamma>`, where:
* `<gradient>`: 0 or 1 (0 is for using same image, 1 is for horizontal gradient);
* `<dithering>`: integer in 0..7, where
  + 0 means No dithering;
  + 1 means Ordered (8x8);
  + 2 means Random;
  + 3 means Floyd–Steinberg;
  + 4 means Jarvis, Judice, Ninke;
  + 5 means Sierra (Sierra-3);
  + 6 means Atkinson;
  + 7 means Halftone (4x4, orthogonal);
* `<bit>`: integer in 1..8  
* `<gamma>`: positive float or 0, where 0 means sRGB using
