; ModuleID = 'unittests/data/hpc4pc_book_fig321.c'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nounwind uwtable
define void @foo() #0 {
entry:
  %k = alloca i32, align 4
  %i = alloca i32, align 4
  call void @llvm.dbg.declare(metadata i32* %k, metadata !10, metadata !12), !dbg !13
  store i32 0, i32* %k, align 4, !dbg !13
  call void @llvm.dbg.declare(metadata i32* %i, metadata !14, metadata !12), !dbg !15
  store i32 0, i32* %i, align 4, !dbg !15
  %0 = load i32, i32* %k, align 4, !dbg !16
  %cmp = icmp sgt i32 %0, 0, !dbg !18
  br i1 %cmp, label %if.then, label %if.end.6, !dbg !19

if.then:                                          ; preds = %entry
  br label %while.cond, !dbg !20

while.cond:                                       ; preds = %if.end, %if.then
  %1 = load i32, i32* %k, align 4, !dbg !22
  %cmp1 = icmp slt i32 %1, 15, !dbg !25
  br i1 %cmp1, label %while.body, label %while.end, !dbg !20

while.body:                                       ; preds = %while.cond
  %2 = load i32, i32* %k, align 4, !dbg !26
  %cmp2 = icmp slt i32 %2, 10, !dbg !29
  br i1 %cmp2, label %if.then.3, label %if.else, !dbg !30

if.then.3:                                        ; preds = %while.body
  %3 = load i32, i32* %k, align 4, !dbg !31
  %inc = add nsw i32 %3, 1, !dbg !31
  store i32 %inc, i32* %k, align 4, !dbg !31
  br label %if.end, !dbg !32

if.else:                                          ; preds = %while.body
  %4 = load i32, i32* %k, align 4, !dbg !33
  %add = add nsw i32 %4, 3, !dbg !33
  store i32 %add, i32* %k, align 4, !dbg !33
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then.3
  %5 = load i32, i32* %k, align 4, !dbg !34
  %inc4 = add nsw i32 %5, 1, !dbg !34
  store i32 %inc4, i32* %k, align 4, !dbg !34
  br label %while.cond, !dbg !20

while.end:                                        ; preds = %while.cond
  br label %g_label, !dbg !35

g_label:                                          ; preds = %while.end
  %6 = load i32, i32* %k, align 4, !dbg !37
  %add5 = add nsw i32 %6, 2, !dbg !37
  store i32 %add5, i32* %k, align 4, !dbg !37
  br label %if.end.6, !dbg !38

if.end.6:                                         ; preds = %g_label, %entry
  br label %h_label, !dbg !39

h_label:                                          ; preds = %if.end.6
  %7 = load i32, i32* %k, align 4, !dbg !41
  %inc7 = add nsw i32 %7, 1, !dbg !41
  store i32 %inc7, i32* %k, align 4, !dbg !41
  ret void, !dbg !42
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 3.7.1 (http://llvm.org/git/clang.git 0dbefa1b83eb90f7a06b5df5df254ce32be3db4b) (http://llvm.org/git/llvm.git 33c352b3eda89abc24e7511d9045fa2e499a42e3)", isOptimized: false, runtimeVersion: 0, emissionKind: 1, enums: !2, subprograms: !3)
!1 = !DIFile(filename: "unittests/data/hpc4pc_book_fig321.c", directory: "/bulk/workbench/repos/pedigree")
!2 = !{}
!3 = !{!4}
!4 = !DISubprogram(name: "foo", scope: !1, file: !1, line: 10, type: !5, isLocal: false, isDefinition: true, scopeLine: 10, isOptimized: false, function: void ()* @foo, variables: !2)
!5 = !DISubroutineType(types: !6)
!6 = !{null}
!7 = !{i32 2, !"Dwarf Version", i32 4}
!8 = !{i32 2, !"Debug Info Version", i32 3}
!9 = !{!"clang version 3.7.1 (http://llvm.org/git/clang.git 0dbefa1b83eb90f7a06b5df5df254ce32be3db4b) (http://llvm.org/git/llvm.git 33c352b3eda89abc24e7511d9045fa2e499a42e3)"}
!10 = !DILocalVariable(tag: DW_TAG_auto_variable, name: "k", scope: !4, file: !1, line: 11, type: !11)
!11 = !DIBasicType(name: "int", size: 32, align: 32, encoding: DW_ATE_signed)
!12 = !DIExpression()
!13 = !DILocation(line: 11, column: 7, scope: !4)
!14 = !DILocalVariable(tag: DW_TAG_auto_variable, name: "i", scope: !4, file: !1, line: 12, type: !11)
!15 = !DILocation(line: 12, column: 7, scope: !4)
!16 = !DILocation(line: 14, column: 7, scope: !17)
!17 = distinct !DILexicalBlock(scope: !4, file: !1, line: 14, column: 7)
!18 = !DILocation(line: 14, column: 9, scope: !17)
!19 = !DILocation(line: 14, column: 7, scope: !4)
!20 = !DILocation(line: 15, column: 5, scope: !21)
!21 = distinct !DILexicalBlock(scope: !17, file: !1, line: 14, column: 14)
!22 = !DILocation(line: 15, column: 12, scope: !23)
!23 = !DILexicalBlockFile(scope: !24, file: !1, discriminator: 3)
!24 = !DILexicalBlockFile(scope: !21, file: !1, discriminator: 1)
!25 = !DILocation(line: 15, column: 14, scope: !21)
!26 = !DILocation(line: 16, column: 11, scope: !27)
!27 = distinct !DILexicalBlock(scope: !28, file: !1, line: 16, column: 11)
!28 = distinct !DILexicalBlock(scope: !21, file: !1, line: 15, column: 20)
!29 = !DILocation(line: 16, column: 13, scope: !27)
!30 = !DILocation(line: 16, column: 11, scope: !28)
!31 = !DILocation(line: 17, column: 10, scope: !27)
!32 = !DILocation(line: 17, column: 9, scope: !27)
!33 = !DILocation(line: 19, column: 11, scope: !27)
!34 = !DILocation(line: 21, column: 8, scope: !28)
!35 = !DILocation(line: 15, column: 5, scope: !36)
!36 = !DILexicalBlockFile(scope: !21, file: !1, discriminator: 2)
!37 = !DILocation(line: 25, column: 7, scope: !21)
!38 = !DILocation(line: 26, column: 3, scope: !21)
!39 = !DILocation(line: 14, column: 11, scope: !40)
!40 = !DILexicalBlockFile(scope: !17, file: !1, discriminator: 1)
!41 = !DILocation(line: 29, column: 4, scope: !4)
!42 = !DILocation(line: 30, column: 1, scope: !4)
