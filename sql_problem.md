
# sakila 問題集

- [sakila 問題集](#sakila-問題集)
  - [penpen](#penpen)
    - [注意](#注意)
      - [対策](#対策)
  - [自作](#自作)


## penpen

[sakilaデータベース用の問題集【MySQL】](https://zenn.dev/yamadadayo123/scraps/9ee01e09aaefd1)

`actor`テーブルから、`first_name`と`last_name`をつなげて`full_name`つくってください。
`first_name`と`last_name`の間は半角スペース1個分を空けてください。
```sql
select concat(first_name, " ", last_name) as full_name from actor;
```

`Egypt`に住んでいる客のフルネームを取得してください
使用するテーブルは`customer`, `address`, `city `, `country `です。
```sql
select 
    concat(c.first_name, " ", c.last_name) as full_name, 
    co.country 
from 
    customer as c 
    join address as a on c.address_id=a.address_id 
    join city on a.city_id=city.city_id 
    join country as co on city.country_id=co.country_id 
where co.country="Egypt";
```

映画のタイトルと、その映画に出演している俳優の数を抽出して、出演している俳優の数が多い順に並び替え、上位10個だけ抽出してください。
使用するテーブルは`film`と`film_actor`です。
```sql
SELECT 
    f.title, 
    COUNT(fa.actor_id) as film_actor_count
FROM 
    film as f
    INNER JOIN film_actor as fa ON f.film_id = fa.film_id
GROUP BY f.title
ORDER BY film_actor_count DESC LIMIT 10;
```

合計180ドル以上支払った顧客の`last_name`を抽出してください。
使用するテーブルは`customer`と`payment`です。
```sql
select 
    c.last_name, 
    sum(p.amount) as total_payment 
from
    customer as c
    join payment as p on p.customer_id=c.customer_id
group by c.customer_id
having total_payment > 180;
```

すべての期間で、売上の合計(`payment.amount`の合計)がトップ5のカテゴリ名を抽出してください。
使用するテーブルは`payment`, `rental`, `inventory`, `film_category`, `category`です。
```sql
select 
    c.name, 
    sum(p.amount) as topsales
from
    payment as p
    join rental as r on p.rental_id = r.rental_id
    join inventory as i on r.inventory_id = i.inventory_id
    join film_category as fc on i.film_id = fc.film_id
    join category as c on fc.category_id = c.category_id
group by c.category_id
order by topsales DESC
limit 5;
```

年別で売上順に全カテゴリを抽出してください。
```sql
select 
    year(p.payment_date) as payment_year, 
    c.name, 
    sum(p.amount) as totalsales
from
    payment as p
    join rental as r on p.rental_id = r.rental_id
    join inventory as i on r.inventory_id = i.inventory_id
    join film_category as fc on i.film_id = fc.film_id
    join category as c on fc.category_id = c.category_id
group by payment_year, c.category_id
order by totalsales DESC;
```

年別で売上がtop5のカテゴリを抽出してください。

```sql
select * from 
(
    select 
        year(p.payment_date) as rental_year, 
        c.name, 
        sum(p.amount) as totalsales,
        rank() OVER 
        (
            PARTITION BY YEAR(r.rental_date) 
            ORDER BY SUM(p.amount) DESC
        ) as ranking
    from
        payment as p
        join rental as r on p.rental_id = r.rental_id
        join inventory as i on r.inventory_id = i.inventory_id
        join film_category as fc on i.film_id = fc.film_id
        join category as c on fc.category_id = c.category_id
    group by rental_year, c.category_id
) as t
where ranking <= 5
order by rental_year ASC, totalsales DESC;
```

### 注意

素朴には次のようなクエリを実行すればいいように思う。

```sql
select 
    year(p.payment_date) as rental_year, 
    c.name, 
    sum(p.amount) as totalsales,
    rank() OVER 
    (
        PARTITION BY YEAR(r.rental_date) 
        ORDER BY SUM(p.amount) DESC
    ) as ranking
from
    payment as p
    join rental as r on p.rental_id = r.rental_id
    join inventory as i on r.inventory_id = i.inventory_id
    join film_category as fc on i.film_id = fc.film_id
    join category as c on fc.category_id = c.category_id
group by rental_year, c.category_id
having ranking <= 5
order by rental_year ASC, totalsales DESC;
```

しかし、上のクエリはエラーに終わる

```sql
ERROR 3594 (HY000): You cannot use the alias 'ranking' of an expression containing a window function in this context.'
```

その理由は、sqlの処理順序にある。

SQLクエリは、次のような順序で処理される：

1. `FROM` (テーブル結合)
2. `WHERE` (行フィルタ)
3. `GROUP BY`  (グループ化)
4. `HAVING` (集約後のグループフィルタ)
5. `SELECT` (列の選択や式の計算 ← ここで`ROW_NUMBER()`が実行される)
6. `ORDER BY`
7. `LIMIT`

- `ROW_NUMBER()`は`SELECT`句で実行されるウィンドウ関数
- `ROW_NUMBER()`の値は、`HAVING`や`WHERE`が評価される時点ではまだ存在してない
- よって`ROW_NUMBER()`の結果を`WHERE`や`HAVING`句で直接使うことはできない

#### 対策

サブクエリを使うことで対策できる

## 自作

各カテゴリごとにstore1でのレンタル数、store2でのレンタル数、及びその合計を降順に抽出してください。

```sql
select
    cg.name,
    sum(
        CASE WHEN c.store_id = 1 THEN 1 ELSE 0 END
    ) AS store1_rental,
    sum(
        CASE WHEN c.store_id = 2 THEN 1 ELSE 0 END
    ) AS store2_rental,
    count(*) as total_rental
from
    rental as r
    join customer as c on r.customer_id=c.customer_id
    join inventory as i on r.inventory_id = i.inventory_id
    join film_category as fc on i.film_id = fc.film_id
    join category as cg on fc.category_id = cg.category_id
group by cg.category_id
order by total_rental desc;
```

2005年下半期に各スタッフが対応したレンタルの件数を抽出せよ。

```sql
select 
    s.staff_id,
    count(r.rental_id)
from
    rental as r
    join staff as s on r.staff_id=s.staff_id
    where year(r.return_date)=2005 and month(r.return_date)>6
group by s.staff_id
```

最も多くの異なるジャンルの映画をレンタルした顧客を求め、その顧客の名前・メールアドレス・ジャンル数を表示してください。
使用するテーブルは`rental`, `inventory`, `film_category`, `category`, `customer`です。
```sql
select 
    c.first_name,
    c.last_name,
    c.email,
    count(DISTINCT cg.category_id) as genre_count
from
    rental as r
    join inventory as i on r.inventory_id = i.inventory_id
    join film_category as fc on i.film_id = fc.film_id
    join category as cg on fc.category_id = cg.category_id
    join customer as c on r.customer_id=c.customer_id
group by c.customer_id
order by genre_count desc
limit 1;
```

全顧客の中で、各月（年＋月単位）における総支払額が最も高かった顧客を月ごとに抽出してください。

```sql
with t as (
    select
        year(p.payment_date) as year,
        month(p.payment_date) as month,
        c.customer_id,
        c.first_name,
        c.last_name,
        sum(amount) as total_paid
    from
        payment as p
        join customer as c on p.customer_id=c.customer_id
    group by year, month, c.customer_id
)
select * 
from 
    t
WHERE NOT EXISTS (
    SELECT 1
    FROM t AS t2
    WHERE 
        t.year = t2.year and t.month=t2.month
        AND t.total_paid < t2.total_paid
);
```

別解

```sql
WITH ranked_payments AS (
    SELECT
        YEAR(p.payment_date) AS year,
        MONTH(p.payment_date) AS month,
        c.customer_id,
        c.first_name,
        c.last_name,
        SUM(p.amount) AS total_paid,
        RANK() OVER (
            PARTITION BY YEAR(p.payment_date), MONTH(p.payment_date) 
            ORDER BY SUM(p.amount) DESC
        ) AS rank_in_month
    FROM
        payment p
        JOIN customer c ON p.customer_id = c.customer_id
    GROUP BY
        YEAR(p.payment_date), MONTH(p.payment_date), c.customer_id
)
SELECT *
FROM ranked_payments
WHERE rank_in_month = 1;
```
