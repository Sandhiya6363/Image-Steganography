# Image Steganography in C

**Hiding data in plain sight using the Least Significant Bit (LSB) technique.**

## About the Project

Image Steganography is a C programming project that demonstrates how secret information can be embedded into a BMP image and retrieved later without noticeably altering the image's appearance.

The project works at the binary level, manipulating individual bits of image data to encode and decode a secret file.

## How It Works

The project uses the **Least Significant Bit (LSB)** technique. The least significant bit of selected image data bytes is modified to store secret information.

### Encoding Process
- Validate command-line arguments and input files.
- Check whether the cover image has enough capacity.
- Copy the BMP header.
- Embed the magic string, secret file extension, file size, and secret data.
- Copy the remaining image data to generate the encoded image.

### Decoding Process
- Read the encoded image.
- Verify the magic string.
- Extract the secret file extension and size.
- Recover the hidden data and save it into an output file.

## Technical Highlights

| Component | Description |
|---|---|
| Language | C |
| Image format | BMP |
| Encoding method | Least Significant Bit (LSB) |
| File operations | Binary read and write |
| Memory concepts | Pointers and structures |
| Data manipulation | Bitwise operations |
| Program interface | Command-line arguments |

## Project Structure

```text
Image-Steganography/
├── main.c
├── encode.c
├── decode.c
├── common.h
├── encode.h
├── decode.h
└── README.md
```

*The file structure above is illustrative; adjust it to match your actual repository.*

## Build and Execute

Compile the required source files with GCC:

```bash
gcc main.c encode.c decode.c -o steganography
```

Example commands, assuming your program supports these arguments:

```bash
./steganography -e input.bmp secret.txt
./steganography -d encoded.bmp
```

Refer to the command-line argument handling in your source code for the exact syntax.

## What I Learned

- Applying bitwise operations to manipulate binary data.
- Working with BMP image headers and image capacity.
- Implementing file encoding and decoding workflows.
- Using pointers, structures, and file handling in C.
- Breaking a complex problem into smaller, reusable functions.

## Why This Project?

This project helped me connect core C programming concepts with practical low-level data processing. It strengthened my understanding of how data is represented, manipulated, and recovered at the bit level.

## Future Improvements

- Support additional image formats.
- Add encryption before embedding secret data.
- Improve error handling and input validation.
- Test the implementation with different file sizes and image capacities.

## Author

**C Programming Project | Image Steganography**

