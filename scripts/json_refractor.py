import json
import xlrd


def replace_title_ref_(ref_excel, curr_json, new_json, json_locale='en', title_index=1, sheet_name='Sheet 1'):
    """
    This function create a new json file which contains english part of the
    title referred from ref_excel file.
    ref_excel: excel file from where name,title for each object will be referred
    curr_json: json file in which titles needed to be replace
    new_json: destination file which will store contain updated titles

    Example:
        replace_title_ref_('mismatched_titles.xlsx', 'original/alphabet_game_map_hi.json', 'updated/alphabet_game_map_hi.json', 'hi', sheet_name='alphabet')
        replace_title_ref_('mismatched_titles.xlsx', 'original/alphabet_game_map_en.json', 'updated/alphabet_game_map_en.json',  sheet_name='alphabet')
    """
    # Read the Excel sheet to create a dictionary
    # newtitle_dict:{name: new_title}
    wb = xlrd.open_workbook(ref_excel)
    sheet = wb.sheet_by_name(sheet_name)
    rowCount = sheet.nrows
    colCount = sheet.ncols
    name_index = -1
    newtitle_index = -1
    for i in range(colCount):
        if name_index == -1 and 'name' == sheet.cell_value(0, i):
            name_index = i
        if newtitle_index == -1 and 'new_title' == sheet.cell_value(0, i):
            newtitle_index = i
        if name_index != -1 and newtitle_index != -1:
            break
    print('Sheet: ', sheet_name)
    print('name_index: ', name_index)
    print('newtitle_index: ', newtitle_index)
    print('Rows count: ', rowCount)
    print('Columns count: ', colCount)
    newtitle_dict = dict()
    for i in range(1, rowCount):
        ref_name = sheet.cell_value(i, name_index)
        ref_title = sheet.cell_value(i, newtitle_index)
        if len(ref_name) > 0:
            newtitle_dict[ref_name] = ref_title
    # Read the json file and replace the english part of the title
    # with new title
    with open(curr_json, 'r', encoding='utf-8-sig') as curr_file:
        check_json_data = json.load(curr_file)
        length = len(check_json_data)
        for i in range(length):
            each_check_object = check_json_data[i]
            check_object_name = each_check_object['name']
            if check_object_name in newtitle_dict:
                ref_object_title = newtitle_dict[check_object_name]
                if json_locale != 'en':
                    check_object_title = each_check_object['title'].split('$#$')
                    check_object_title[title_index] = ref_object_title
                    new_check_object_title = '$#$'.join(check_object_title)
                else:
                    new_check_object_title = ref_object_title
                each_check_object['title'] = new_check_object_title
            check_json_data[i] = each_check_object
    with open(new_json, 'w', encoding='utf-8') as new_jsonfile:
        json.dump(check_json_data, new_jsonfile, ensure_ascii=False, indent=2)

# English titles in _en and _hi jsons 
replace_title_ref_('mismatched_titles.xlsx', 'original/alphabet_game_map_hi.json', 'en_updated/alphabet_game_map_hi.json', 'hi', sheet_name='alphabet')
replace_title_ref_('mismatched_titles.xlsx', 'original/alphabet_game_map_en.json', 'en_updated/alphabet_game_map_en.json',  sheet_name='alphabet')
replace_title_ref_('mismatched_titles.xlsx', 'original/grammar_game_map_hi.json', 'en_updated/grammar_game_map_hi.json', 'hi', sheet_name='grammar')
replace_title_ref_('mismatched_titles.xlsx', 'original/grammar_game_map_en.json', 'en_updated/grammar_game_map_en.json',  sheet_name='grammar')
replace_title_ref_('mismatched_titles.xlsx', 'original/shapes_game_map_hi.json', 'en_updated/shapes_game_map_hi.json', 'hi', sheet_name='shapes')
replace_title_ref_('mismatched_titles.xlsx', 'original/shapes_game_map_en.json', 'en_updated/shapes_game_map_en.json',  sheet_name='shapes')
replace_title_ref_('mismatched_titles.xlsx', 'original/writing_game_map_hi.json', 'en_updated/writing_game_map_hi.json', 'hi', sheet_name='writing')
replace_title_ref_('mismatched_titles.xlsx', 'original/writing_game_map_en.json', 'en_updated/writing_game_map_en.json',  sheet_name='writing')

# Hindi titles _hi jsons 
replace_title_ref_('game_titles.xlsx', 'en_updated/alphabet_game_map_hi.json', 'hi_updated/alphabet_game_map_hi.json', 'hi', title_index=0,  sheet_name='alphabet')
replace_title_ref_('game_titles.xlsx', 'en_updated/grammar_game_map_hi.json', 'hi_updated/grammar_game_map_hi.json', 'hi', title_index=0, sheet_name='grammar')
replace_title_ref_('game_titles.xlsx', 'en_updated/shapes_game_map_hi.json', 'hi_updated/shapes_game_map_hi.json', 'hi', title_index=0, sheet_name='shapes')
replace_title_ref_('game_titles.xlsx', 'en_updated/writing_game_map_hi.json', 'hi_updated/writing_game_map_hi.json', 'hi', title_index=0, sheet_name='writing')
