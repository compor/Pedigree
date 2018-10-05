; RUN: rm -f %bindir/tests/mdg.foo.dot
; RUN: opt -load %bindir/%testeelib -basicaa -pedigree-mdg-dfs-enumerate -pedigree-mdg-dot < %s --disable-output
; RUN: %checkgraphiso -f %bindir/tests/mdg.foo.dot %outputdatadir/rar_mdg.dot -a dg_uid | FileCheck %s
; CHECK: OK

define void @foo() {
entry:
  %0 = load i8*, i8** null, align 8
  %1 = load i8*, i8** null, align 8
  ret void
}
