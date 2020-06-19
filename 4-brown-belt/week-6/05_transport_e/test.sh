./a.out < test_data/1_input.txt > test.txt
diff test_data/1_output.txt test.txt

./a.out < test_data/2_input.txt > test.txt
diff test_data/2_output.txt test.txt

./a.out < test_data/3_input.txt > test.txt
diff test_data/3_output.txt test.txt

./a.out < test_data/4_input.txt > test.txt
diff test_data/4_output.txt test.txt
