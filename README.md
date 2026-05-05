
# Xi-Dach---Nhom-3---IT002.Q26

## I. Giới thiệu chung

Dự án này là một hệ thống mô phỏng, thống kê và rèn luyện AI "chuyên nghiệp" cho hai trò chơi Casino/"dân gian" phổ biến: **BlackJack** (Tiêu chuẩn quốc tế) và **Xì Dách** (Luật Việt Nam).

Dự án này không chỉ nhằm mục đích tạo ra một trò chơi giải trí mà còn là một **công cụ nghiên cứu xác suất** thông qua việc giả lập hàng triệu ván bài, ghi nhận dữ liệu thống kê chuyên sâu và tự động tinh chỉnh chiến thuật bằng các thuật toán học máy cơ bản.

## II. Tính Năng Nổi Bật

### 1.  Core Engine

-   Quản lý bộ bài (52 lá) với các thao tác xào bài, chia bài ngẫu nhiên (sử dụng thư viện `<random>` và `mt19937` cho độ phân tán cao).
-   Hệ thống quản lý thực thể (`Hand`, `Player`, `Dealer`) xử lý Hard Hand, Soft Hand, Print Hand, tính điểm, tính các thông số bool, ...

### 2. BlackJack

-   Hỗ trợ đầy đủ các hành động chuẩn mực: **Hit, Stand, Double Down, Split, Surrender** và **Insurance**.
-   Tích hợp hệ thống AI gợi ý chiến thuật (AI Suggest) dựa trên biểu đồ chiến thuật cơ bản (Basic Strategy). AI có thể tự động quyết định hành động tối ưu dựa vào quân bài ngửa của nhà cái (Dealer's Face-up card) và điểm của người chơi.

### 3. Xì Dách Việt Nam

-   Tích hợp trọn vẹn luật chơi truyền thống Việt Nam với các loại bài đặc biệt: **Xì Bàng** (2 lá Ách), **Xì Dách** (Ách + 10/J/Q/K), và **Ngũ Linh** (5 lá tổng điểm <= 21).
-   **Logic Xét Bài Của Nhà Cái:** Tái hiện chính xác thực tế khi nhà cái (Dealer) có quyền chọn xét bài của từng người chơi ngay trước khi rút bài hoặc trong quá trình rút bài của mình.
-   **Reinforcement Learning (`MatchXDAI`):** Ứng dụng hệ thống học tăng cường sau mỗi vòng mô phỏng để rút ra chiến thuật tốt nhất

### 4.  Thống kê dữ liệu

-   **Báo cáo toàn diện:** Đếm gần như tất cả mọi thông số mà các "chuyên gia" có thể cần khi phân tích chuyến thuật
-   **Xuất dữ liệu:** Sau khi chạy mô phỏng, code sẽ xuất file `.csv` để trực quan hoá dữ liệu bằng Excel

----------

##  III. Cấu Trúc Mã Nguồn

-   `Constants.cpp/.h`: Cấu hình hệ thống, ánh xạ giá trị bài (Rank) và chất bài (Suit: ♠, ♣, ♢, ♡), hàm hỗ trợ xuất file.
-   `Entities.cpp/.h`: Khai báo các đối tượng trò chơi. Class `Hand` xử lý logic tính điểm tay bài; Class `Player` chứa danh sách các tay bài (cho phép Tách bài - Split); Class `Dealer` in và ẩn bài của nhà cái.
-   `Engine.cpp/.h`: Vận hành khung xương trò chơi (`Match`), quản lý vòng đời bộ bài và thiết lập người chơi.
-   `MatchBJ.cpp/.h`: Vận hành luật BlackJack
-   `MatchXD.cpp/.h`: Vận hành luật Xì Dách Việt Nam
-   `Stats.cpp/.h`: Theo dõi và thu thập dữ liệu thống kê (`Stat`, `StatBJ`, `StatXD`). Chứa các tính toán toán học về Expected Value (EV) và logic tạo tập tin CSV báo cáo.

----------



