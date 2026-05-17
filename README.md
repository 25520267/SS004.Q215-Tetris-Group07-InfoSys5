# 🎮 Tetris C++ - Đồ án môn Kỹ năng nghề nghiệp

Một phiên bản làm lại của tựa game xếp hình Tetris huyền thoại, được phát triển bằng ngôn ngữ C++ chạy trên nền Console của Windows. Đây là đồ án nhóm dành cho môn Kỹ năng nghề nghiệp tại UIT.

## 🌟 Các tính năng nổi bật
* **Cơ chế cốt lõi:** Đầy đủ các khối gạch chuẩn, xoay khối, ghi điểm, xóa dòng và tăng cấp độ (Level up).
* **Ghost Piece (Bóng mờ):** Hiển thị trước vị trí khối gạch sẽ rơi xuống, giúp người chơi căn chỉnh chính xác.
* **Hard Drop (Thả nhanh):** Nhấn phím để thả khối gạch rơi ngay lập tức xuống đáy.
* **Hold Piece (Cất khối):** Cho phép người chơi cất giữ một khối gạch để sử dụng chiến thuật về sau.
* **Hệ thống Next Piece:** Hiển thị trước các khối gạch tiếp theo theo thuật toán 7-bag chuẩn.
* **Giao diện trực quan:** Bảng điểm, cấp độ và màu sắc riêng biệt cho từng loại khối gạch.

## 🕹️ Cách điều khiển
* `A` / `D` : Di chuyển khối gạch sang Trái / Phải
* `S` : Soft Drop (Rơi nhanh hơn)
* `W` : Xoay khối gạch (Rotate)
* `Space` : Hard Drop (Thả rơi ngay lập tức)
* `C` : Hold Piece (Cất khối / Lấy khối đã cất)
* `Q` : Thoát game (Exit)

## 💻 Yêu cầu hệ thống (Prerequisites)
* **Hệ điều hành:** Yêu cầu Windows (do game sử dụng các thư viện đặc thù của Windows như `<windows.h>`, `<conio.h>`).
* **Trình biên dịch:** Hỗ trợ bất kỳ trình biên dịch C++ nào (MinGW/GCC) hoặc có thể chạy trực tiếp thông qua Visual Studio.

## 🚀 Hướng dẫn Cài đặt & Chạy game
1. **Sử dụng Visual Studio (Khuyên dùng):**
   - Mở file `Tetris_InfoSys5.slnx` hoặc thư mục dự án bằng Visual Studio.
   - Nhấn `F5` hoặc nút **Local Windows Debugger** để biên dịch và chạy game.
2. **Sử dụng Terminal / MinGW:**
   - Mở Terminal (Command Prompt / PowerShell) tại thư mục `Tetris_InfoSys5`.
   - Gõ lệnh biên dịch: `g++ main.cpp -o tetris.exe`
   - Chạy game: `.\tetris.exe`

## 👥 Danh sách thành viên nhóm

| STT | Họ và Tên | MSSV |
| :---: | :--- | :---: |
| 1 | Lê Dương Quốc Đạt | 25520267 |
| 2 | Nguyễn Trương Phúc Đạt | 25520291 |
| 3 | Lê Thiện Ân | 25520014 |
| 4 | Huỳnh Trọng Khang | 25520781 |
| 5 | Nguyễn Ngô Thanh Liêm | 25520992 |


