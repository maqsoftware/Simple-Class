/*
 * Copyright 2017, Team Chimple
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.maq.xprize.bali.db.dao;

import android.arch.persistence.room.Dao;
import android.arch.persistence.room.Insert;
import android.arch.persistence.room.OnConflictStrategy;
import android.arch.persistence.room.Query;

import com.maq.xprize.bali.db.entity.Unit;

@Dao
public interface UnitDao {
    @Query("SELECT * FROM Unit WHERE name = :name AND type = :type")
    Unit getUnitByNameAndType(String name, int type);

    @Query("SELECT * FROM Unit WHERE id = :id")
    Unit getUnitById(Long id);

    @Query("SELECT COUNT(*) FROM Unit")
    int count();

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    Long insertUnit(Unit unit);

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    Long[] insertUnits(Unit... units);
}
