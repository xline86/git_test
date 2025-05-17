# vba standard memo

- [vba standard memo](#vba-standard-memo)
  - [配列](#配列)
  - [繰り返し](#繰り返し)
  - [条件分岐](#条件分岐)
  - [覚えておきたい ↓](#覚えておきたい-)
  - [ファイル操作](#ファイル操作)
  - [ワークシート関数](#ワークシート関数)
  - [セル](#セル)
    - [検索](#検索)
    - [移動](#移動)
  - [フィルター](#フィルター)
  - [並べ替え](#並べ替え)
  - [テーブル](#テーブル)
  - [エラー](#エラー)
  - [デバッグ](#デバッグ)


## 配列

```vb
dim a() as Variant
redim a(1)
a(1)="a"
redim preserve a(2)
a=array(0,1,2)

Dim list(3 To 5) As Integer
For i = LBound(list) To UBound(list)
    list(i) = i * 2
Next
```

## 繰り返し

```vb
do {while|until} 条件式
    [exit do]
loop

For counter = start To end [Step 幅]
    [Exit For]
Next
```

## 条件分岐

```vb
Select Case score
Case 100
    MsgBox "満点"
Case Is >= 90
    MsgBox "秀"
case 70 to 89
    msgbox "優"
case 100
    msgbox "これは実行されない"
case else
    msgbox "それ以外"
End Select
```

## 覚えておきたい ↓

```vb
Selection
ActiveCell
ThisWorkbook
Nothing
TypeName(str)
'  文字列操作
Format(Now, "yyyy")
Year(Now)
Replace(文字列, 変換元, 変換先)
Split("2025-05-05","-")
```

## ファイル操作

```vb
FileCopy "path1", "path2"
```
```vb
' ブックを閉じる
ThisWorkbook.Close SaveChanges:=True
' 保存
ThisWorkbook.Save
' 名前を付けて保存 保存先が新しいアクティブなブックになる
Thisworkbook.SaveAs "C:\User\sample.xlsm"
' コピーを保存 バックアップの作成に使われる
ThisWorkbook.SaveCopyAs "path"
```
```vb
' ブックを開く
workbooks.open "folder_path/" & thisworkbook.sheets(1).cells(1,1)
```
```vb
' mkdir
For i = 1 To 3
    MkDir ThisWorkbook.Path & "/" & i
Next
```
```vb
If Dir("C:\test", vbDirectory) = "" Then
    MsgBox "C:\testなるディレクトリは存在しない"
End If

Name = Dir(ThisWorkbook.Path & "/test_*.txt")
Do While Name <> ""
    Debug.Print Name
    Name = Dir
Loop
' test_1.txt
' test_2.txt
```


## ワークシート関数

WorksheetFunction
- match
- index
- large

## セル

### 検索

```vb
t = cells.find(what:="a", LookAt:={xlWhol|xlpart})
```

### 移動

range.end
```vb
Range("a1").End(xlToLeft)
' xlDown
' xlToLeft
' xlToRight
' xlU
```

## フィルター
autofilter
```vb
range("a1").autofilter 1, "a"
range("table1").autofilter 1, "a", xland, "b"
' Operator:=[xland, xlor, xlFilterValues]
ary = Array("a","b","c")
range("table1[#data]").autofilter 1, ary, Operator:=xlFilterValues
```

## 並べ替え
sort object
```vb
With ActiveSheet.Sort
    .SortFields.Clear
    .SortFields.Add2 Key:=Range("C2"), Order:=xlDescending
    ' xlAscending, xlDescending
    .DataOption:=xlsortnormal
    .SetRange Range("table1")
    .Header = xlyes
    .Apply
End With
```

sort method(2003年まで)
```vb
range("table1[#data]").sort key1="a", order1=xlascending, key2="b", Header=xlyes
```

## テーブル

listobject
```vb
Worksheets(1).ListObjects("table1").DataBodyRange.Select
Cells(4, 4).ListObject.HeaderRowRange.Select
Debug.Print Worksheets(1).ListObjects("table1").ListColumns(1)
```

ListObjects object
メソッド
- Add: 新しいlistobjectを作成
property
- Count
- Item

ListObject collection
メソッド
- Unlist: リストとしての機能を削除
property
- AutoFilter: 値の取得のみ可能
- DataBodyRange: 値
- HeaderRowRange: 見出し行
- ListColumns: 各列
- ListRows: 各行
- Name
- Range: 全てのセル
- ShowAutoFilter: Boolean
- ShowTotals: Boolean
- Sort
- TableStyle
- TotalsRowRange: 集計行

```vb
Sub table_op()
    With Worksheets(1).ListObjects("table1")
        ' 値の設定
        .ListColumns(2).Range(2) = "あ"
        ' 列の追加
        .ListColumns.Add Position:=3
        .ListColumns(3).Name = "cat"
        .DataBodyRange.Columns(3).Value = "=CONCAT([@c列],[@d列])"
    End With
End Sub

Sub create_table()
    Dim tbl As ListObject
    Set tbl = Worksheets(1).ListObjects.Add( _
        SourceType:=xlSrcRange, _
        Source:=Range("a1").CurrentRegion, _
        XlListObjectHasHeaders:=xlYes)
    tbl.Name = "table1"
End Sub

Sub row_delete()
  With Range("A1").ListObject.DataBodyRange
    .AutoFilter 2, "ss"
    ' 条件に合う行を削除
    .EntireRow.Delete
  End With
  Range("A1").AutoFilter 2

End Sub
```

## エラー

- 記述エラー
- 論理エラー
  - コンパイルエラー
  - 実行エラー

err object
メソッド
- Clear
- Raise: 実行エラーを生成
オブジェクト
- Description: 説明
- Number: エラーを表す規定の数字

`On Error Resume Next`
エラーを無視して進む
```vb
Sub error_test()
On Error GoTo hoge
    Cells(0, 0).Select
Exit Sub
hoge:
    Debug.Print Err.Number
    Debug.Print Err.Description
End Sub
```

## デバッグ

```vb
' デバッグ
Stop
```
- デバッグモード: 中断しているとき
- イミディエイトウィンドウ
  - ctrl + g
  - ?変数名
