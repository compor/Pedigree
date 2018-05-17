
define void @foo() {
entry:
  call void @llvm.memset.p0i8.i64(i8* undef, i8 0, i64 40, i32 16, i1 false)
  unreachable
}

declare void @llvm.memset.p0i8.i64(i8* nocapture, i8, i64, i32, i1)

