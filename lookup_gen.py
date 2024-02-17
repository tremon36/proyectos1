import math

def write_to_file(string, file_path):
    try:
        with open(file_path, 'a') as file:
            file.write(string)  
    except IOError:
        print("Error: Unable to write to file", file_path)

def generate(n_points,fn,PWM_n_points):
    for i in range(0,n_points):
        write_to_file(f"{min(math.floor(fn((2*math.pi/n_points) * i) * (PWM_n_points // 2)) + (PWM_n_points // 2),PWM_n_points-1)}, ","lut.txt")
        if(i%15 == 0 and i != 0):
            write_to_file("\n","lut.txt")

def main():
    npoints = 10000
    write_to_file(f"uint16_t LUT[{npoints}] = " + "{","lut.txt")
    generate(npoints,math.sin,16536)
    write_to_file(" };\n","lut.txt")

if(__name__ == "__main__"):
    main()

    