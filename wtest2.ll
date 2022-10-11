; ModuleID = 'WPLC.ll'
source_filename = "WPLC.ll"

@test = private unnamed_addr constant [1 x i8] zeroinitializer, align 1

define i32 @t1() {
t1head:
  br label %enter

enter:                                            ; preds = %t1head
  %test = alloca i8*, align 8
  store i8* getelementptr inbounds ([1 x i8], [1 x i8]* @test, i32 0, i32 0), i8** %test, align 8
  ret i32 4
}

define i8* @c(i8* %n) {
chead:
  %n1 = alloca i8*, align 8
  store i8* %n, i8** %n1, align 8
  br label %enter

enter:                                            ; preds = %chead
  %n2 = load i8*, i8** %n1, align 8
  ret i8* %n2
}
