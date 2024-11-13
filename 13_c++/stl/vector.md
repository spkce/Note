
# 不建议使用vector<bool>
而是通过一个策略把bool值压缩为位(bit)，这样可以极大地节省存储空间.std::vector<bool>的行为也可能与std::vector的行为有些不同，这可能会导致一些奇怪和非预期的结果