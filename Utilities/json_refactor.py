"""
This script updates the Hindi and English titles of the games in the JSON
"""
import json
import xlrd


def replace_title_ref_(ref_excel, curr_json, new_json, json_locale='en', title_index=1, sheet_name='Sheet 1'):
    """
    This function creates a new JSON file which contains English part of the
    title referred from ref_excel file.
    ref_excel: Excel file from where name, title for each object will be referred
    curr_json: JSON file in where titles needed to be replace
    new_json: destination file which will store updated titles

    Example:
        replace_title_ref_('mismatched_titles.xlsx', 'original/alphabet_game_map_hi.json', 'updated/alphabet_game_map_hi.json', 'hi', sheet_name='alphabet')
        replace_title_ref_('mismatched_titles.xlsx', 'original/alphabet_game_map_en.json', 'updated/alphabet_game_map_en.json',  sheet_name='alphabet')
    """
    # delimiter between Hindi and English Title
    dem_char = '$#$'
    # identity key(column name/key) for the object in the JSON and Excel
    ident_key = 'name'
    # column name for the new title
    src_key = 'new_title'
    # key of the target that needed to be changed
    target_key = 'title'
    # Read the Excel sheet to create a dictionary
    wb = xlrd.open_workbook(ref_excel)
    sheet = wb.sheet_by_name(sheet_name)
    row_count = sheet.nrows
    col_count = sheet.ncols
    # Get the column index of identity and new title column in Excel
    name_index = -1
    newtitle_index = -1
    for i in range(col_count):
        if name_index == -1 and ident_key == sheet.cell_value(0, i):
            name_index = i
        if newtitle_index == -1 and src_key == sheet.cell_value(0, i):
            newtitle_index = i
        if name_index != -1 and newtitle_index != -1:
            break
    print('Sheet: ', sheet_name)
    print('name_index: ', name_index)
    print('newtitle_index: ', newtitle_index)
    print('Rows count: ', row_count)
    print('Columns count: ', col_count)
    newtitle_dict = dict()
    # Create a dictionary between the identity key and the new value
    for i in range(1, row_count):
        ref_name = sheet.cell_value(i, name_index)
        ref_title = sheet.cell_value(i, newtitle_index)
        if len(ref_name) > 0:
            newtitle_dict[ref_name] = ref_title
    # Read the JSON file and replace the english part of the title
    # with new title using the dictionary
    with open(curr_json, 'r', encoding='utf-8-sig') as curr_file:
        check_json_data = json.load(curr_file)
        length = len(check_json_data)
        for i in range(length):
            each_check_object = check_json_data[i]
            check_object_name = each_check_object[ident_key]
            if check_object_name in newtitle_dict:
                ref_object_title = newtitle_dict[check_object_name]
                if json_locale != 'en':
                    check_object_title = each_check_object[target_key].split(dem_char)
                    check_object_title[title_index] = ref_object_title
                    new_check_object_title = dem_char.join(check_object_title)
                else:
                    new_check_object_title = ref_object_title
                each_check_object[target_key] = new_check_object_title
            check_json_data[i] = each_check_object
    # Create a new JSON file with updated name
    with open(new_json, 'w', encoding='utf-8') as new_jsonfile:
        json.dump(check_json_data, new_jsonfile, ensure_ascii=False, indent=2)

file_root = 'original/'
updated_en = 'en_updated/'
updated_hi = 'hi_updated/'
locales = ['en', 'hi']
files = ['alphabet_game_map', 'grammar_game_map', 'shapes_game_map', 'writing_game_map']
sheets = ['alphabet', 'grammar', 'shapes', 'writing']

# Update the English titles in all the JSONs files
mismatched_titles_excel = 'mismatched_titles.xlsx'
for locale in locales:
    for i in range(0, len(files)):
        replace_title_ref_(mismatched_titles_excel, file_root+files[i]+'_'+locale+'.json', updated_en+files[i]+'_'+locale+'.json', locale, 0, sheets[i])

# Update the Hindi titles in the Hindi JSONs files
hindi_titles_excel = 'game_titles.xlsx'
hi_index = 1
for file in files:
    replace_title_ref_(hindi_titles_excel, updated_en+file+'_'+locales[hi_index]+'.json', updated_hi+file+'_'+locales[hi_index]+'.json', locale, 1, sheets[i])
