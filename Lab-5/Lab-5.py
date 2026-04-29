import streamlit as st
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os
import re

st.set_page_config(layout="wide", page_title="Аналіз даних VHI - Лабораторна №5")

@st.cache_data
def load_and_clean_data(folder_path="vhi_data"):
    all_dfs = []
    
    province_names = {
        1: 'Cherkasy', 2: 'Chernihiv', 3: 'Chernivtsi', 4: 'Crimea',
        5: 'Dnipropetrovsk', 6: 'Donetsk', 7: 'Ivano-Frankivsk',
        8: 'Kharkiv', 9: 'Kherson', 10: 'Khmelnytskyy',
        11: 'Kyiv', 12: 'Kyiv City', 13: 'Kirovohrad',
        14: 'Luhansk', 15: 'Lviv', 16: 'Mykolayiv',
        17: 'Odessa', 18: 'Poltava', 19: 'Rivne',
        20: 'Sevastopol', 21: 'Sumy', 22: 'Ternopil',
        23: 'Transcarpathia', 24: 'Vinnytsya', 25: 'Volyn',
        26: 'Zaporizhzhya', 27: 'Zhytomyr'
    }

    if not os.path.exists(folder_path):
        st.error(f"Папка '{folder_path}' не знайдена!")
        return pd.DataFrame()

    for file_name in os.listdir(folder_path):
        if not (file_name.lower().startswith("vhi_") and file_name.endswith(".csv")):
            continue

        try:
            province_id = int(file_name.split('_')[1])
            full_path = os.path.join(folder_path, file_name)

            df = pd.read_csv(
                full_path,
                header=1,
                names=['year', 'week', 'SMN', 'SMT', 'VCI', 'TCI', 'VHI', 'empty'],
                skipinitialspace=True,
                engine='python'
            )

            df = df.drop(columns=['empty'], errors='ignore')
            
            df['year'] = df['year'].astype(str).str.replace(r'<[^>]*>', '', regex=True).str.strip()
            
            df = df.apply(pd.to_numeric, errors='coerce')
            df = df.replace(-1, pd.NA).dropna()

            df['year'] = df['year'].astype(int)
            df['week'] = df['week'].astype(int)
            df['province_id'] = province_id
            df['Province'] = province_names.get(province_id, f"Unknown_{province_id}")

            all_dfs.append(df)
            
        except (ValueError, IndexError, Exception) as e:
            continue

    return pd.concat(all_dfs, ignore_index=True) if all_dfs else pd.DataFrame()

df = load_and_clean_data()

def reset_all_filters():
    st.session_state.index_key = "VHI"
    st.session_state.province_key = df['Province'].unique()[0] if not df.empty else None
    st.session_state.weeks_key = (int(df['week'].min()), int(df['week'].max()))
    st.session_state.years_key = (int(df['year'].min()), int(df['year'].max()))
    st.session_state.asc_key = False
    st.session_state.desc_key = False

if 'index_key' not in st.session_state:
    reset_all_filters()

col_sidebar, col_display = st.columns([1, 3])

with col_sidebar:
    st.header("Фільтри")
    
    selected_index = st.selectbox("Оберіть показник:", ["VCI", "TCI", "VHI"], key="index_key")
    
    all_provinces = sorted(df['Province'].unique())
    selected_province = st.selectbox("Оберіть область:", all_provinces, key="province_key")
    
    week_range = st.slider("Інтервал тижнів:", 
                           int(df['week'].min()), int(df['week'].max()), 
                           key="weeks_key")
    
    year_range = st.slider("Інтервал років:", 
                           int(df['year'].min()), int(df['year'].max()), 
                           key="years_key")
    
    st.button("Скинути всі фільтри", on_click=reset_all_filters)
    
    st.markdown("---")
    st.subheader("Сортування")
    sort_asc = st.checkbox("За зростанням", key="asc_key")
    sort_desc = st.checkbox("За спаданням", key="desc_key")

    if sort_asc and sort_desc:
        st.warning("Обрано обидва типи сортування. Пріоритет: Зростання.")

time_mask = (
    (df['year'] >= year_range[0]) & (df['year'] <= year_range[1]) &
    (df['week'] >= week_range[0]) & (df['week'] <= week_range[1])
)

filtered_df = df[time_mask & (df['Province'] == selected_province)].copy()

if sort_asc:
    filtered_df = filtered_df.sort_values(by=selected_index, ascending=True)
elif sort_desc:
    filtered_df = filtered_df.sort_values(by=selected_index, ascending=False)

with col_display:
    tab_table, tab_plot, tab_compare = st.tabs(["Таблиця", "Графік області", "Порівняння"])
    
    with tab_table:
        st.subheader(f"Дані для області {selected_province}")
        st.dataframe(filtered_df, use_container_width=True)
        
    with tab_plot:
        st.subheader(f"Часовий ряд {selected_index} ({selected_province})")
        if not filtered_df.empty:
            fig, ax = plt.subplots(figsize=(10, 4))
            sns.lineplot(data=filtered_df, x='year', y=selected_index, marker='o', ax=ax)
            ax.set_ylabel(selected_index)
            ax.grid(True, alpha=0.3)
            st.pyplot(fig)
        else:
            st.info("Немає даних для графіка за вказаними фільтрами.")
            
    with tab_compare:
        st.subheader(f"Порівняння {selected_index} між областями")
        comp_df = df[time_mask].copy()
        if not comp_df.empty:
            fig2, ax2 = plt.subplots(figsize=(10, 6))
            avg_df = comp_df.groupby('Province')[selected_index].mean().reset_index()
            sns.barplot(data=avg_df, x='Province', y=selected_index, palette="viridis", ax=ax2)
            plt.xticks(rotation=90)
            st.pyplot(fig2)
        else:
            st.info("Оберіть період, де є дані для порівняння.")