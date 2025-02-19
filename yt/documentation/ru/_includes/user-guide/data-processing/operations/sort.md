# Sort

Операция Sort сортирует данные из указанных таблиц и записывает их в одну выходную таблицу. Для сортированной таблицы доступна операция Reduce, а при чтении данных из таблицы (в частности, с указанием диапазона) они будут идти в сортированном виде. Также появляется возможность использовать диапазоны (range) по значениям, а не только по номерам строк таблицы. Сравнение сначала выполняется по типу, а затем, при совпадении типов, — обычным образом (числа по значению, строки лексикографически). 

{% note info "Примечание" %}

В операции сортировки нет гарантий на порядок строк с одинаковым значением ключа.

{% endnote %}

Одним из основных параметров сортировки является количество партиций (`partition_count`). Данный параметр вычисляется заранее. Перед запуском основных джобов из входной таблицы вычитываются выборки, с помощью которых вычисляются диапазоны ключей, разбивающие входные данные на части (по числу `partition_count`) примерно одинакового размера. Дальнейшая работа выполняется в несколько фаз:

1. На первой фазе запускаются partition джобы, которые разбивают весь свой вход на `partition_count` партиций, определяя, к какой партиции относится каждая строка;
2. На второй фазе запускаются sorting джобы, которые сортируют данные из полученных партиций. Существуют две опции: если данных в партиции мало, то она сортируется целиком, и на этом обработка партиции заканчивается. Если же данных много и одним джобом их отсортировать не получится, sorting джобы запускаются на частях фиксированного размера данной партиции, далее наступает третья фаза.
3. На третьей фазе, запускаются merge джобы, которые сливают сортированные части больших партиций.

Общие параметры для всех типов операций описаны в разделе [Настройки операций](../../../../user-guide/data-processing/operations/operations-options.md).

У операции Sort поддерживаются следующие дополнительные параметры (в скобках указаны значения по умолчанию, если заданы):

* `sort_by` — список колонок, по которым будет производиться сортировка. Обязательный параметр;
* `input_table_paths` — список входных таблиц с указанием полных путей (не может быть пустым);
* `output_table_path` — полный путь к выходной таблице;
* `partition_count`, `partition_data_size` — опции, которые указывают, сколько партиций должно быть сделано в сортировке, имеют рекомендательный характер;
* `partition_job_count`, `data_size_per_partition_job` — опции, которые указывают сколько partition джобов должно быть запущено, имеют рекомендательный характер;
* `intermediate_data_replication_factor` (1) — коэффициент репликации промежуточных данных;
* `intermediate_data_account` (intermediate) — аккаунт, в квоте которого будут учитываться промежуточные данные сортировки;
* `intermediate_compression_codec` (lz4) — кодек, используемый для сжатия промежуточных данных;
* `intermediate_data_medium` (`default`) — тип носителя, на котором хранятся чанки промежуточных данных сортировки;
* `partition_job_io, sort_job_io, merge_job_io` — IO-настройки соответствующих типов джобов; секция `table_writer` в опции `merge_job_io` добавляется ко всем джобам, которые пишут в выходные таблицы; 
* `schema_inference_mode` (auto) — режим определения схемы. Доступные значения: auto, from_input, from_output. Подробнее можно прочитать в разделе [Схема данных](../../../../user-guide/storage/static-schema.md#schema_inference);
* `samples_per_partition` (1000) — количество ключей для выборок из таблицы для каждой партиции (доступно только для динамических таблиц);
* `data_size_per_sorted_merge_job` — опция, регулирующая количество данных на входе в merge джобах, имеет рекомендательный характер;
* `sort_locality_timeout` (1 min) — время, в течение которого планировщик будет ждать появления свободных ресурсов на конкретных узлах кластера, чтобы запускать сортировку всех частей каждой партиции на одном узле. Данное действие необходимо для обеспечения большей локальности чтения при последующем слиянии сортированных данных.

По умолчанию `partition_count` и `partition_job_count` вычисляются автоматически, исходя из объема исходных данных, так, чтобы минимизировать время выполнения сортировки.

## Пример спецификации

Пример спецификации Sort операции:

```yaml
{
  data_size_per_partition_job = 1000000;
  sort_by = ["key"; "subkey" ];
  input_table_paths = [ "//tmp/input_table1"; "//tmp/input_table2" ];
  output_table_path = "//tmp/sorted_output_table";
}
```
