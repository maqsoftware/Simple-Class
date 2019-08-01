import json
import xlsxwriter


def get_title_miss_matches(ref_json, check_json, xlsx_file):
    """
    This function create an excel sheet which contains all the miss-matched
    titles in the check_json file.
    ref_json: file from where name,title for each object will be referred
    check_json: file which titles needed to be checked
    xlsx_file: excel file that will be contains all the mismatched titles
    <name> <en_json_title> <hi_json_title_eng>

    Example:
        get_title_miss_matches('alphabet_game_map_en.json', 'alphabet_game_map_hi.json', 'DifferentTitles.xlsx')
    """
    wb = xlsxwriter.Workbook(xlsx_file)
    ref_json_dict = dict()
    with open(ref_json, 'r', encoding='utf-8-sig') as ref_file:
        ref_json_data = json.load(ref_file)
        for each_object in ref_json_data:
            ref_json_dict[each_object['name']] = each_object
    sheet = wb.add_worksheet("Sheet 1")

    rowCount = 0
    sheet.write(rowCount, 0, 'name')
    sheet.write(rowCount, 1, 'en_json_title')
    sheet.write(rowCount, 2, 'hi_json_title_eng')
    # sheet.write(rowCount, 3, 'hi_json_title_hi')
    rowCount = rowCount+2
    with open(check_json, 'r', encoding='utf-8-sig') as check_file:
        check_json_data = json.load(check_file)
        for each_check_object in check_json_data:
            check_object_name = each_check_object['name']
            check_object_title = each_check_object['title'].split('$#$')
            ref_object = ref_json_dict[check_object_name]
            ref_object_title = ref_object['title']
            if ref_object_title != check_object_title[1]:
                sheet.write(rowCount, 0, check_object_name)
                sheet.write(rowCount, 1, ref_object_title)
                sheet.write(rowCount, 2, check_object_title[1])
                rowCount = rowCount+1
            # sheet.write(rowCount, 3, check_object_title[0])
    wb.close()


def replace_title_ref_(ref_json, check_json, dest_file):
    """
    This function create a new json file which contains english part of the
    title referred from ref_json file.
    ref_json: file from where name,title for each object will be referred
    check_json: file which titles needed to be checked
    dest_file: destination file which will store contain updated objects
    <name> <en_json_title> <hi_json_title_eng>

    Example:
        replace_title_ref_('alphabet_game_map_en.json', 'alphabet_game_map_hi.json', 'alphabet_game_map_hi_new.json')
    """
    ref_json_dict = dict()
    rowCount = 0
    with open(ref_json, 'r', encoding='utf-8-sig') as ref_file:
        ref_json_data = json.load(ref_file)
        for each_object in ref_json_data:
            ref_json_dict[each_object['name']] = each_object
    rowCount = rowCount+1
    with open(check_json, 'r', encoding='utf-8-sig') as check_file:
        check_json_data = json.load(check_file)
        length = len(check_json_data)
        for i in range(length):
            each_check_object = check_json_data[i]
            check_object_name = each_check_object['name']
            check_object_title = each_check_object['title'].split('$#$')
            ref_object = ref_json_dict[check_object_name]
            ref_object_title = ref_object['title']
            new_check_object_title = check_object_title[0]+'$#$'+ref_object_title
            each_check_object['title'] = new_check_object_title
            check_json_data[i] = each_check_object
    with open(dest_file, 'w', encoding='utf-8') as destfile:
        json.dump(check_json_data, destfile, ensure_ascii=False, indent=2)


def get_titles_name_json(ref_json, check_json, xlsx_file):
    """
    This function create an Excel sheet which contain name and title from ref_json, english and hindi separated title
    from the check_json file
    ref_json: file from where name,title for each object will be referred
    check_json: file which titles needed to be checked
    xlsx_file: excel file that will be contains the mentioned field from above

    <name> <en_json_title> <hi_json_title_eng> <hi_json_title_hi>

    Example:
        get_titles_name_json('alphabet_game_map_en.json', 'alphabet_game_map_hi.json', 'TitlesandNames.xlsx')
    """
    wb = xlsxwriter.Workbook(xlsx_file)
    ref_json_dict = dict()
    with open(ref_json, 'r', encoding='utf-8-sig') as ref_file:
        ref_json_data = json.load(ref_file)
        for each_object in ref_json_data:
            ref_json_dict[each_object['name']] = each_object
    sheet = wb.add_worksheet("Sheet 1")

    rowCount = 0
    sheet.write(rowCount, 0, 'name')
    sheet.write(rowCount, 1, 'en_json_title')
    sheet.write(rowCount, 2, 'hi_json_title_eng')
    sheet.write(rowCount, 3, 'hi_json_title_hi')
    rowCount = rowCount+2
    with open(check_json, 'r', encoding='utf-8-sig') as check_file:
        check_json_data = json.load(check_file)
        for each_check_object in check_json_data:
            check_object_name = each_check_object['name']
            check_object_title = each_check_object['title'].split('$#$')
            ref_object = ref_json_dict[check_object_name]
            ref_object_title = ref_object['title']
            sheet.write(rowCount, 0, check_object_name)
            sheet.write(rowCount, 1, ref_object_title)
            sheet.write(rowCount, 2, check_object_title[1])
            sheet.write(rowCount, 3, check_object_title[0])
            rowCount = rowCount+1
    wb.close()
