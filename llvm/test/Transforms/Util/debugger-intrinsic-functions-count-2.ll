; RUN: opt -disable-output -passes=difcp %s 2>&1 | FileCheck %s

; CHECK: {{^}}Function: mult{{$}}
; CHECK: {{^}}llvm.dbg.declare: 3{{$}}

; ModuleID = 'multiply.c'
source_filename = "multiply.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @mult() #0 !dbg !10 {
entry:
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %c = alloca i32, align 4
  call void @llvm.dbg.declare(metadata ptr %a, metadata !15, metadata !DIExpression()), !dbg !16
  store i32 5, ptr %a, align 4, !dbg !16
  call void @llvm.dbg.declare(metadata ptr %b, metadata !17, metadata !DIExpression()), !dbg !18
  store i32 3, ptr %b, align 4, !dbg !18
  call void @llvm.dbg.declare(metadata ptr %c, metadata !19, metadata !DIExpression()), !dbg !20
  %0 = load i32, ptr %a, align 4, !dbg !21
  %1 = load i32, ptr %b, align 4, !dbg !22
  %mul = mul nsw i32 %0, %1, !dbg !23
  store i32 %mul, ptr %c, align 4, !dbg !20
  %2 = load i32, ptr %b, align 4, !dbg !24
  %add = add nsw i32 %2, 5, !dbg !25
  store i32 %add, ptr %a, align 4, !dbg !26
  %3 = load i32, ptr %c, align 4, !dbg !27
  ret i32 %3, !dbg !28
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "clang version 17.0.0 (https://github.com/rakicaleksandar1999/llvm-project 1e99e70cdd1aeb172bf568abf80f080355d35c7a)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "multiply.c", directory: "/home/syrmia/Desktop/LLVM/github/llvm-project/tmp", checksumkind: CSK_MD5, checksum: "6012136a13aad31e1d4cad6461a74aab")
!2 = !{i32 7, !"Dwarf Version", i32 5}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 2}
!8 = !{i32 7, !"frame-pointer", i32 2}
!9 = !{!"clang version 17.0.0 (https://github.com/rakicaleksandar1999/llvm-project 1e99e70cdd1aeb172bf568abf80f080355d35c7a)"}
!10 = distinct !DISubprogram(name: "mult", scope: !1, file: !1, line: 1, type: !11, scopeLine: 1, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!11 = !DISubroutineType(types: !12)
!12 = !{!13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !{}
!15 = !DILocalVariable(name: "a", scope: !10, file: !1, line: 2, type: !13)
!16 = !DILocation(line: 2, column: 6, scope: !10)
!17 = !DILocalVariable(name: "b", scope: !10, file: !1, line: 3, type: !13)
!18 = !DILocation(line: 3, column: 6, scope: !10)
!19 = !DILocalVariable(name: "c", scope: !10, file: !1, line: 4, type: !13)
!20 = !DILocation(line: 4, column: 6, scope: !10)
!21 = !DILocation(line: 4, column: 10, scope: !10)
!22 = !DILocation(line: 4, column: 14, scope: !10)
!23 = !DILocation(line: 4, column: 12, scope: !10)
!24 = !DILocation(line: 5, column: 6, scope: !10)
!25 = !DILocation(line: 5, column: 7, scope: !10)
!26 = !DILocation(line: 5, column: 4, scope: !10)
!27 = !DILocation(line: 6, column: 9, scope: !10)
!28 = !DILocation(line: 6, column: 2, scope: !10)
