import sys
import zipfile
import gzip

def read_file(filename):
    """Reads a .txt, .zip, or .gz file and returns a list of tuples (result, first_prime, second_prime)."""
    data = []
    
    # Handle .gz (gzip) files
    if filename.endswith('.gz'):
        try:
            with gzip.open(filename, 'rt') as file:  # 'rt' mode reads as text
                for line in file:
                    parts = line.strip().replace(' ', '').split('=')
                    if len(parts) != 2:
                        continue
                    result = int(parts[0])
                    primes = parts[1].split('+')
                    if len(primes) != 2:
                        continue
                    first_prime = int(primes[0])
                    second_prime = int(primes[1])
                    data.append((result, first_prime, second_prime))
        except gzip.BadGzipFile:
            print(f"Error: '{filename}' is not a valid gzip file.")
            return None
        except ValueError:
            print("Error: Invalid format in the gzip file.")
            return None
    
    # Handle .zip files
    elif filename.endswith('.zip'):
        try:
            with zipfile.ZipFile(filename, 'r') as z:
                # Assume there's only one .txt file in the ZIP
                txt_files = [f for f in z.namelist() if f.endswith('.txt')]
                if not txt_files:
                    print("Error: No .txt file found in the ZIP archive.")
                    return None
                if len(txt_files) > 1:
                    print("Error: Multiple .txt files found in the ZIP. Please include only one.")
                    return None
                
                # Read the single .txt file from the ZIP
                with z.open(txt_files[0], 'r') as file:
                    # Decode bytes to string
                    lines = file.read().decode('utf-8').splitlines()
                    for line in lines:
                        parts = line.strip().replace(' ', '').split('=')
                        if len(parts) != 2:
                            continue
                        result = int(parts[0])
                        primes = parts[1].split('+')
                        if len(primes) != 2:
                            continue
                        first_prime = int(primes[0])
                        second_prime = int(primes[1])
                        data.append((result, first_prime, second_prime))
        except zipfile.BadZipFile:
            print(f"Error: '{filename}' is not a valid ZIP file.")
            return None
        except ValueError:
            print("Error: Invalid format in the file inside the ZIP.")
            return None
    
    # Handle regular .txt files
    else:
        try:
            with open(filename, 'r') as file:
                for line in file:
                    parts = line.strip().replace(' ', '').split('=')
                    if len(parts) != 2:
                        continue
                    result = int(parts[0])
                    primes = parts[1].split('+')
                    if len(primes) != 2:
                        continue
                    first_prime = int(primes[0])
                    second_prime = int(primes[1])
                    data.append((result, first_prime, second_prime))
        except FileNotFoundError:
            print(f"Error: The file '{filename}' was not found.")
            return None
        except ValueError:
            print("Error: Invalid format in the file.")
            return None
    
    return data

def calculate_frequencies(data, max_value):
    """Calculates the frequencies of values in the second column up to the maximum value."""
    second_column = [first_prime for (result, first_prime, _) in data if result <= max_value]
    frequency_dict = {}
    for num in second_column:
        frequency_dict[num] = frequency_dict.get(num, 0) + 1
    return frequency_dict

def main():
    # Check if the correct number of command-line arguments is provided
    if len(sys.argv) != 3:
        print("Usage: python goldbach_freq.py input_file max")
        print("  - input_file: Name of the .txt, .zip (containing a single .txt), or .gz file")
        print("  - max: Maximum value of the first column (an even number >= 6)")
        return
    
    # Get arguments
    filename = sys.argv[1]
    try:
        max_value = int(sys.argv[2])
    except ValueError:
        print("Error: 'max' must be an integer.")
        print("Usage: python goldbach_freq.py input_file max")
        return
    
    # Validate max_value
    if max_value < 6 or max_value % 2 != 0:
        print("Error: 'max' must be an even number greater than or equal to 6.")
        print("Usage: python goldbach_freq.py input_file max")
        return
    
    # Read the file
    data = read_file(filename)
    if data is None:
        return
    
    # Calculate frequencies
    frequencies = calculate_frequencies(data, max_value)
    
    # Display results
    print("\nValues in the second column and their frequencies:")
    for value, freq in sorted(frequencies.items()):
        print(f"Value: {value}, Frequency: {freq}")

if __name__ == "__main__":
    main()
