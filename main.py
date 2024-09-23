import requests
import json
import pandas as pd
import time

class PichauBase:
    url = "https://www.pichau.com.br/api/catalog"
    sku_list = [
        'PCM-Pichau-Gamer-50695',
        '100-100000927BOX',
        'BX8071514400',
        'BX8071514100',
        '100-100001488BOX',
        '100-100001503WOF',
        'BX8071512400F-BR',
        'CMK16GX4M2D3000C16',
        'CMW16GX4M2D3600C18W',
        'CMT16GX4M2D3600C18W',
        'GV-N4060WF2OC-8GD',
        'GV-N406TWF2OC-8GD',
        'VCG40608DFXPB1-O',
        '912-V515-098'
    ]

    headers = {
        'accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7',
        'accept-language': 'pt-BR,pt;q=0.9,en-US;q=0.8,en;q=0.7',
        'priority': 'u=0, i',
        'referer': 'https://www.google.com/',
        'user-agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Safari/537.36 OPR/111.0.0.0',
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Safari/537.36 OPR/111.0.0.0',
        'Origin': 'https://www.pichau.com.br',
        'content-type': 'application/json',
        'origin': 'https://www.pichau.com.br',
        'cachettl': '76800',
        'if-none-match': 'W/"24a-qxS3F0FyEAdXCooDWpyX/IVq6Vg"',
        'pichaucachekey': 'ratings-review',
        'Accept': '*/*',
        'Service-Worker': 'script',
        'pragma': 'no-cache',
    }

    @classmethod
    def make_requests(cls):
        session = requests.Session()
        for sku in cls.sku_list:
            body = {
                "operationName": "productDetail",
                "variables": {"sku": sku},
                "query": "query productDetail($sku: String) {\n  productDetail: products(filter: {sku: {eq: $sku}}) {\n    items {\n      __typename\n      sku\n      name\n      only_x_left_in_stock\n      stock_status\n      special_price\n      mysales_promotion {\n        expire_at\n        price_discount\n        price_promotional\n        promotion_name\n        promotion_url\n        qty_available\n        qty_sold\n        __typename\n      }\n      pichauUlBenchmarkProduct {\n        overallScore\n        scoreCPU\n        scoreGPU\n        games {\n          fullHdFps\n          medium4k\n          quadHdFps\n          title\n          ultra1080p\n          ultra4k\n          __typename\n        }\n        __typename\n      }\n      pichau_prices {\n        avista\n        avista_discount\n        avista_method\n        base_price\n        final_price\n        max_installments\n        min_installment_price\n        __typename\n      }\n      price_range {\n        __typename\n      }\n      ... on BundleProduct {\n        dynamic_sku\n        dynamic_price\n        dynamic_weight\n        price_view\n        ship_bundle_items\n        options: items {\n          option_id\n          title\n          required\n          type\n          position\n          sku\n          value: options {\n            id\n            uid\n            quantity\n            position\n            is_default\n            price\n            price_type\n            can_change_quantity\n            title: label\n            product {\n              id\n              name\n              sku\n              url_key\n              stock_status\n              slots_memoria\n              portas_sata\n              image {\n                url\n                url_listing\n                path\n                label\n                __typename\n              }\n              __typename\n            }\n            __typename\n          }\n          __typename\n        }\n        __typename\n      }\n    }\n    __typename\n  }\n}\n"
            }

            try:
                response = session.post(cls.url, headers=cls.headers, json=body)
                response.raise_for_status()
                print(f"Sucesso para SKU {sku}: {response.status_code}")
                data = response.json()
                # Processar os dados conforme necessário
                # Por exemplo: data_df = pd.DataFrame(data['data']['productDetail']['items'])
                # data_df.to_json(f"{sku}_data.json", orient='records')
            except requests.RequestException as e:
                print(f"Erro na requisição para SKU {sku}: {e}")
            
            time.sleep(5)

if __name__ == "__main__":
    PichauBase.make_requests()