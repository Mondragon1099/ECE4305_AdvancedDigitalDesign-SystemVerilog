# lab-4-rom-based-temperature-conversion-seanw_danielm

<img width="682" height="378" alt="Screenshot 2026-02-16 232517" src="https://github.com/user-attachments/assets/cde28165-9c7a-4a59-ac8d-416ed54b6864" />

7.7.2 ROM-based temperature conversion
Temperature can be measured in Celsius or Fahrenheit scale. Let c and f be a temperature
reading in Celsius and Fahrenheit scales. They are related by
𝑓 = 9
5 × 𝑐 + 32
The conversion involves multiplication and division operations and direct implementation
requires a significant amount of hardware resource. For a simple application, such as a digital
thermometer, we can create a lookup table for conversion and store it in a ROM.
Consider a conversion circuit with following specification:
• The range is between 0°C and 100°C (32°F and 212°F).
• The input and output are in 8-bit unsigned format.
• A separate format signal indicates whether the input is in Celsius or Fahrenheit scale. The
output is to be converted to the other scale.
We can create two lookup tables for the two conversions. Note that because of the small size of
these tables, it is possible to store the two tables in a single BRAM module. Design the circuit
and verify its operation.
Notes:
• Your FPGA implementation should show the input and output temperatures on the
seven-segment display
• If you input a number out of range (i.e., 5°F) the output should display 0
• Your implementation must use BRAM and the usage must not exceed 1 BRAM.
(Technically, you can implement the whole system using 0.5 BRAM but 1 BRAM is
acceptable)
• You should only use 1 ROM memory module (i.e., instantiate it as many times as you
need but there is only one version). You can use my ROM code and modify it so the
mem file name is parameterized)
