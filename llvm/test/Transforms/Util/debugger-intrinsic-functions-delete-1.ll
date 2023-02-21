; RUN: opt -disable-output -passes=difdp %s 2>&1 | FileCheck %s

; CHECK: {{^}}define i32 @foo() {{{$}}
; CHECK: {{^}}  %a = add i32 2, 3{{$}}
; CHECK: {{^}}  ret i32 %a{{$}}
; CHECK: {{^}}}{{$}}
define i32 @foo() {
  %a = add i32 2, 3
  ret i32 %a
}


; CHECK: {{^}}define void @bar() {{{$}}
; CHECK: {{^}}  ret void{{$}}
; CHECK: {{^}}}{{$}}
define void @bar() {
  ret void
}
