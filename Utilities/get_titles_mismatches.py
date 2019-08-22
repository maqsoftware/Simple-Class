"""
This script has functions to create Excel sheets for the mismatched titles,
to replace titles from JSON reference file and also contains an Excel function
with titles from Hindi JSON file
"""
import json
import xlsxwriter


def get_title_miss_matches(ref_json, check_json, xlsx_file, sheet_name="Sheet 1"):
    """
    This function creates an Excel sheet which contains all the mismatched
    titles in the check_json file.
    ref_json: JSON file from where name, title for each object will be referred
    check_json: JSON file where titles needed to be checked
    xlsx_file: Excel file that will contain all the mismatched titles
    <name> <en_json_title> <hi_json_title_eng>

    Example:
        get_title_miss_matches('alphabet_game_map_en.json', 'alphabet_game_map_hi.json', 'DifferentTitles.xlsx')
    """
    dem_char = '$#$'
    ident_key = 'name'
    target_key = 'title'
    wb = xlsxwriter.Workbook(xlsx_file)
    ref_json_dict = dict()
    # Open the JSON and create a map with the ident_key
    with open(ref_json, 'r', encoding='utf-8-sig') as ref_file:
        ref_json_data = json.load(ref_file)
        for each_object in ref_json_data:
            ref_json_dict[each_object[ident_key]] = each_object
    sheet = wb.add_worksheet(sheet_name)
    row_count = 0
    sheet.write(row_count, 0, 'name')
    sheet.write(row_count, 1, 'en_json_title')
    sheet.write(row_count, 2, 'hi_json_title_eng')
    row_count = row_count+2
    # Write to Excel file all the mismatched titles
    with open(check_json, 'r', encoding='utf-8-sig') as check_file:
        check_json_data = json.load(check_file)
        for each_check_object in check_json_data:
            check_object_name = each_check_object[ident_key]
            check_object_title = each_check_object[target_key].split(dem_char)
            ref_object = ref_json_dict[check_object_name]
            ref_object_title = ref_object[target_key]
            if ref_object_title != check_object_title[1]:
                sheet.write(row_count, 0, check_object_name)
                sheet.write(row_count, 1, ref_object_title)
                sheet.write(row_count, 2, check_object_title[1])
                row_count = row_count+1
    wb.close()


def replace_title_ref_(ref_json, check_json, dest_file):
    """
    This function creates a new JSON file which contains english part of the
    title referred from ref_json file.
    ref_json: JSON file from where name,title for each object will be referred
    check_json: JSON file where titles needed to be checked
    dest_file: destination JSON file which will contains updated objects

    Example:
        replace_title_ref_('alphabet_game_map_en.json', 'alphabet_game_map_hi.json', 'alphabet_game_map_hi_new.json')
    """
    dem_char = '$#$'
    ident_key = 'name'
    target_key = 'title'
    ref_json_dict = dict()
    # Create map from the English JSON file
    with open(ref_json, 'r', encoding='utf-8-sig') as ref_file:
        ref_json_data = json.load(ref_file)
        for each_object in ref_json_data:
            ref_json_dict[each_object[ident_key]] = each_object
    # Update the JSON data from the ref JSON file
    with open(check_json, 'r', encoding='utf-8-sig') as check_file:
        check_json_data = json.load(check_file)
        length = len(check_json_data)
        for i in range(length):
            each_check_object = check_json_data[i]
            check_object_name = each_check_object[ident_key]
            check_object_title = each_check_object[target_key].split(dem_char)
            ref_object = ref_json_dict[check_object_name]
            ref_object_title = ref_object[target_key]
            new_check_object_title = check_object_title[0] + ident_key + ref_object_title
            each_check_object[target_key] = new_check_object_title
            check_json_data[i] = each_check_object
    # Create a new JSON file from the file
    with open(dest_file, 'w', encoding='utf-8') as destfile:
        json.dump(check_json_data, destfile, ensure_ascii=False, indent=2)


def get_titles_name_json(ref_json, check_json, xlsx_file, sheet_name="Sheet 1"):
    """
    This function creates an Excel sheet which contains name and titles from ref_json, english and hindi separated title
    from the check_json file
    ref_json: file from where name, title for each object will be referred
    check_json: file where titles needed to be checked
    xlsx_file: Excel file that will be contain, the above mentioned field

    <name> <en_json_title> <hi_json_title_eng> <hi_json_title_hi>

    Example:
        get_titles_name_json('alphabet_game_map_en.json', 'alphabet_game_map_hi.json', 'TitlesandNames.xlsx')
    """
    dem_char = '$#$'
    ident_key = 'name'
    target_key = 'title'
    wb = xlsxwriter.Workbook(xlsx_file)
    ref_json_dict = dict()
    # Read the JSON file for the reference
    with open(ref_json, 'r', encoding='utf-8-sig') as ref_file:
        ref_json_data = json.load(ref_file)
        for each_object in ref_json_data:
            ref_json_dict[each_object[ident_key]] = each_object
    sheet = wb.add_worksheet(sheet_name)

    row_count = 0
    sheet.write(row_count, 0, 'name')
    sheet.write(row_count, 1, 'en_json_title')
    sheet.write(row_count, 2, 'hi_json_title_eng')
    sheet.write(row_count, 3, 'hi_json_title_hi')
    row_count = row_count+2
    # Write to Excel file all the titles in the JSON file
    with open(check_json, 'r', encoding='utf-8-sig') as check_file:
        check_json_data = json.load(check_file)
        for each_check_object in check_json_data:
            check_object_name = each_check_object[ident_key]
            check_object_title = each_check_object[target_key].split(dem_char)
            ref_object = ref_json_dict[check_object_name]
            ref_object_title = ref_object[target_key]
            sheet.write(row_count, 0, check_object_name)
            sheet.write(row_count, 1, ref_object_title)
            sheet.write(row_count, 2, check_object_title[1])
            sheet.write(row_count, 3, check_object_title[0])
            row_count = row_count+1
    wb.close()

locales = ['en', 'hi']
file_root = 'original/'
excel_path = 'check_excels/'
files = ['alphabet_game_map', 'grammar_game_map', 'shapes_game_map', 'writing_game_map']
for file in files:
    get_titles_name_json(file_root+file+'_'+locales[0]+'.json', file_root+file+'_'+locales[1]+'.json', excel_path+file+'.xlsx')
for file in files:
    get_title_miss_matches(file_root+file+'_'+locales[0]+'.json', file_root+file+'_'+locales[1]+'.json', excel_path+file+'.xlsx')
