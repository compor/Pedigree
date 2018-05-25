; RUN: rm -f %bindir/tests/mdg.foo1.dot
; RUN: opt -load %bindir/%testeelib -pedigree-mdg-dot < %s --disable-output
; RUN: %checkgraphiso %bindir/tests/mdg.foo1.dot %outputdatadir/mdamdgbuilder_scope_bug01.dot | FileCheck %s
; CHECK: OK

define void @foo1() {
entry:
  call void @llvm.memset.p0i8.i64(i8* undef, i8 0, i64 40, i32 16, i1 false)
  unreachable
}

declare void @llvm.memset.p0i8.i64(i8* nocapture, i8, i64, i32, i1)

