***系統架構

BMP image
  ↓
[Encoder]
  - Parse BMP header
  - Normalize pixel order (top-down)
  - Split B/G/R channels
  - Save image metadata (dim.txt)
  ↓
R.txt / G.txt / B.txt / dim.txt
  ↓
[Decoder]
  - Reconstruct BMP headers
  - Restore original pixel order
  - Write BGR pixel data with padding
  ↓
Reconstructed BMP image


***實作進度與工作紀錄

- Implemented BMP header parsing with packed structs
- Handled bottom-up and top-down BMP formats
- Correctly processed row padding (4-byte alignment)
- Successfully reconstructed BMP with bit-exact output
- Verified correctness using cmp


***Workflow 與 Artifacts 說明

GitHub Actions workflow is used to automatically:
- Compile encoder and decoder
- Run encoder to generate channel text files
- Run decoder to reconstruct BMP image
- Upload important outputs as artifacts

Artifacts include:
- R.txt, G.txt, B.txt
- dim.txt
- Reconstructed BMP image


***心得與反思

Through this project, I gained a deeper understanding of binary file formats
and how image data is represented at the byte level.
Implementing both encoder and decoder clarified the separation between
image semantics and file container structures.
Handling BMP row padding and pixel ordering was particularly insightful.
